#include "extensions/filters/network/memcached_proxy/binary_protocol_decoder_impl.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MemcachedProxy {

bool
BinaryProtocolDecoderImpl::decode(Buffer::Instance& data) {
  ENVOY_LOG(trace, "received {} bytes to decode", data.length());

  // See if we have enough data to decode the binary protocol header
  if (data.length() < BinaryCodec::HeaderSize) {
    return false;
  }
  setPkgIdentifier(data);

  decodeHeader(data);

  decodeBody(data);
  if (data.length() > 0) {
    ENVOY_LOG(error, "{} bytes remaining will be drained", data.length());
    data.drain(data.length());
  }

  return true;
}

void
BinaryProtocolDecoderImpl::decodeBody(Buffer::Instance& data) {
  ENVOY_LOG(trace, "extras: {}", drainString(data, extras_length_));
  ENVOY_LOG(trace, "key={}", drainString(data, key_length_));
  ENVOY_LOG(trace, "body={}", drainString(data, total_body_length_));
}

void
BinaryProtocolDecoderImpl::decodeHeader(Buffer::Instance& data) {
  auto op_code = data.drainBEInt<uint8_t>();
  auto key_length = data.drainBEInt<uint16_t>();
  auto extras_length = data.drainBEInt<uint8_t>();
  auto data_type = data.drainBEInt<uint8_t>();
  auto vbucket_id_or_status = data.drainBEInt<uint16_t>();
  auto total_body_length = data.drainBEInt<uint32_t>();
  auto opaque = data.drainBEInt<uint32_t>();
  auto cas = data.drainBEInt<uint64_t>();

  command_op_code_ = op_code;
  key_length_ = key_length;
  extras_length_ = extras_length;
  data_type_ = data_type;
  total_body_length_ = total_body_length;
  opaque_ = opaque;
  cas_ = cas;

  ENVOY_LOG(trace, "command op code: {}", op_code);
  ENVOY_LOG(trace, "key length: {}", key_length);
  ENVOY_LOG(trace, "extras length: {}", extras_length);
  ENVOY_LOG(trace, "data type: {}", data_type);
  ENVOY_LOG(trace, "vbucket ID or status: {}", vbucket_id_or_status);
  ENVOY_LOG(trace, "total body length: {}", total_body_length);
  ENVOY_LOG(trace, "opaque: {}", opaque);
  ENVOY_LOG(trace, "cas: {}", cas);

  setCommand(static_cast<BinaryCodec::CommandOpCode>(op_code));
  if (BinaryCodec::PkgIdentifier::RESPONSE == pkg_identifier_) {
    auto response_status = static_cast<BinaryCodec::ResponseStatus>(vbucket_id_or_status);
    verifyResponseStatus(response_status);
  }

  callbacks_.decodedCommand(command_);
  callbacks_.decodedTotalBodyLength(total_body_length);
}

std::string
BinaryProtocolDecoderImpl::drainString(Buffer::Instance& data, uint32_t string_length) {
  if (data.length() < string_length || data.length() == 0 || string_length == 0) {
    return {};
  }

  char* start = reinterpret_cast<char*>(data.linearize(string_length));
  std::string ret(start, string_length);
  data.drain(string_length);
  return ret;
}

void
BinaryProtocolDecoderImpl::encodeDecoderError(Buffer::Instance& buffer) {
  std::string error_message = "decoder error";

  buffer.writeByte(BinaryCodec::PkgIdentifier::RESPONSE);
  buffer.writeByte(command_op_code_);
  buffer.writeBEInt<uint16_t>(0x0000); // Key length
  buffer.writeByte(0x00); // Extras length
  buffer.writeByte(data_type_);
  buffer.writeBEInt<uint16_t>(static_cast<uint8_t>(BinaryCodec::ResponseStatus::INTERNAL_ERROR));
  buffer.writeBEInt<uint32_t>(error_message.length()); // Total body length
  buffer.writeBEInt<uint32_t>(opaque_);
  buffer.writeBEInt<uint64_t>(cas_);
  buffer.add(error_message);
}

void
BinaryProtocolDecoderImpl::onData(Buffer::Instance& data) {
  while (data.length() > 0 && decode(data)) {}
}

void
BinaryProtocolDecoderImpl::setCommand(BinaryCodec::CommandOpCode op_code) {
  switch (op_code) {
  case BinaryCodec::CommandOpCode::GET:
    command_ = Codec::Command::GET;
    ENVOY_LOG(trace, "`GET`");
    break;
  case BinaryCodec::CommandOpCode::SET:
    command_ = Codec::Command::SET;
    ENVOY_LOG(trace, "`SET`");
    break;
  case BinaryCodec::CommandOpCode::ADD:
    command_ = Codec::Command::ADD;
    ENVOY_LOG(trace, "`ADD`");
    break;
  case BinaryCodec::CommandOpCode::REPLACE:
    command_ = Codec::Command::REPLACE;
    ENVOY_LOG(trace, "`REPLACE`");
    break;
  case BinaryCodec::CommandOpCode::DELETE:
    command_ = Codec::Command::DELETE;
    ENVOY_LOG(trace, "`DELETE`");
    break;
  case BinaryCodec::CommandOpCode::INCREMENT:
    command_ = Codec::Command::INCREMENT;
    ENVOY_LOG(trace, "`INCREMENT`");
    break;
  case BinaryCodec::CommandOpCode::DECREMENT:
    command_ = Codec::Command::DECREMENT;
    ENVOY_LOG(trace, "`DECREMENT`");
    break;
  case BinaryCodec::CommandOpCode::VERSION:
    command_ = Codec::Command::VERSION;
    ENVOY_LOG(trace, "`VERSION`");
    break;
  case BinaryCodec::CommandOpCode::GET_K:
    command_ = Codec::Command::GET_K;
    ENVOY_LOG(trace, "`GET_K`");
    break;
  case BinaryCodec::CommandOpCode::APPEND:
    command_ = Codec::Command::APPEND;
    ENVOY_LOG(trace, "`APPEND`");
    break;
  case BinaryCodec::CommandOpCode::PREPEND:
    command_ = Codec::Command::PREPEND;
    ENVOY_LOG(trace, "`PREPEND`");
    break;
  default:
    throw DecoderError(fmt::format("Unsupported op code {}", static_cast<uint8_t>(op_code)));
  }
}

void
BinaryProtocolDecoderImpl::setPkgIdentifier(Buffer::Instance& data) {
  auto pkg_identifier = static_cast<BinaryCodec::PkgIdentifier>(data.peekBEInt<uint8_t>());
  if (BinaryCodec::PkgIdentifier::REQUEST != pkg_identifier &&
      BinaryCodec::PkgIdentifier::RESPONSE != pkg_identifier) {
    ENVOY_LOG(info, "Unable to interpret data using the binary protocol");
    throw ProtocolError("pkg identifier does not match binary protocol");
  }

  ENVOY_LOG(trace, "pkg identifier: {}", static_cast<uint8_t>(pkg_identifier));

  if (BinaryCodec::PkgIdentifier::REQUEST == pkg_identifier) {
    callbacks_.decodedRequest();
  }
  else if (BinaryCodec::PkgIdentifier::RESPONSE == pkg_identifier) {
    callbacks_.decodedResponse();
  }

  pkg_identifier_ = pkg_identifier;
  data.drain(sizeof(uint8_t)); // Avoid re-reading this byte
}

void
BinaryProtocolDecoderImpl::verifyResponseStatus(BinaryCodec::ResponseStatus response_status) {
  switch (response_status) {
  case BinaryCodec::ResponseStatus::NO_ERROR:
  case BinaryCodec::ResponseStatus::KEY_NOT_FOUND:
  case BinaryCodec::ResponseStatus::KEY_EXISTS:
  case BinaryCodec::ResponseStatus::VALUE_TOO_LARGE:
  case BinaryCodec::ResponseStatus::INVALID_ARGUMENTS:
  case BinaryCodec::ResponseStatus::ITEM_NOT_STORED:
  case BinaryCodec::ResponseStatus::INCR_DECR_ON_NON_NUMERIC_VALUE:
  case BinaryCodec::ResponseStatus::VBUCKET_BELONGS_TO_ANOTHER_SERVER:
  case BinaryCodec::ResponseStatus::AUTHENTICATION_ERROR:
  case BinaryCodec::ResponseStatus::AUTHENTICATION_CONTINUE:
  case BinaryCodec::ResponseStatus::UNKNOWN_COMMAND:
  case BinaryCodec::ResponseStatus::OUT_OF_MEMORY:
  case BinaryCodec::ResponseStatus::NOT_SUPPORTED:
  case BinaryCodec::ResponseStatus::INTERNAL_ERROR:
  case BinaryCodec::ResponseStatus::BUSY:
  case BinaryCodec::ResponseStatus::TEMPORARY_FAILURE:
    break;
  default:
    throw DecoderError(fmt::format("Unsupported response status {}", static_cast<uint16_t>(response_status)));
  }
}

} // namespace MemcachedProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
