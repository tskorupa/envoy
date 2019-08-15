#pragma once

#include "common/buffer/buffer_impl.h"
#include "common/common/logger.h"

#include "extensions/filters/network/memcached_proxy/codec.h"
#include "extensions/filters/network/memcached_proxy/decoder.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MemcachedProxy {

class BinaryCodec {
public:
  constexpr static uint8_t HeaderSize = 24 * sizeof(uint8_t);

  enum class PkgIdentifier {
    REQUEST = 0x80,
    RESPONSE = 0x81,
  };

  enum class CommandOpCode {
    GET = 0x00,
    SET = 0x01,
    ADD = 0x02,
    REPLACE = 0x03,
    DELETE = 0x04,
    INCREMENT = 0x05,
    DECREMENT = 0x06,
    VERSION = 0x0b,
    GET_K = 0x0c,
    APPEND = 0x0e,
    PREPEND = 0x0f,
  };

  enum class ResponseStatus {
    NO_ERROR = 0x0000,
    KEY_NOT_FOUND = 0x0001,
    KEY_EXISTS = 0x0002,
    VALUE_TOO_LARGE = 0x0003,
    INVALID_ARGUMENTS = 0x0004,
    ITEM_NOT_STORED = 0x0005,
    INCR_DECR_ON_NON_NUMERIC_VALUE = 0x0006,
    VBUCKET_BELONGS_TO_ANOTHER_SERVER = 0x0007,
    AUTHENTICATION_ERROR = 0x0008,
    AUTHENTICATION_CONTINUE = 0x0009,
    UNKNOWN_COMMAND = 0x0081,
    OUT_OF_MEMORY = 0x0082,
    NOT_SUPPORTED = 0x0083,
    INTERNAL_ERROR = 0x0084,
    BUSY = 0x0085,
    TEMPORARY_FAILURE = 0x0086,
  };
};

class BinaryProtocolDecoderImpl : public Decoder, Logger::Loggable<Logger::Id::memcached> {
public:
  BinaryProtocolDecoderImpl(DecoderCallbacks& callbacks) : callbacks_(callbacks) {}

  void onData(Buffer::Instance& data) override;
  void encodeDecoderError(Buffer::Instance& buffer) override;

  Codec::Command command_;
  BinaryCodec::PkgIdentifier pkg_identifier_;
  uint8_t command_op_code_;
  uint16_t key_length_;
  uint8_t extras_length_;
  uint8_t data_type_;
  uint32_t total_body_length_;
  uint32_t opaque_;
  uint64_t cas_;

private:
  bool decode(Buffer::Instance& data);
  void decodeBody(Buffer::Instance& data);
  void decodeHeader(Buffer::Instance& data);
  std::string drainString(Buffer::Instance& data, uint32_t string_length);
  void setCommand(BinaryCodec::CommandOpCode op_code);
  void setPkgIdentifier(Buffer::Instance& data);
  void verifyResponseStatus(BinaryCodec::ResponseStatus response_status);

  DecoderCallbacks& callbacks_;
};

} // namespace MemcachedProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
