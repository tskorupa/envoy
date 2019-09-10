#include "extensions/filters/network/memcached_proxy/filter.h"

#include "common/common/assert.h"
#include "common/common/logger.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MemcachedProxy {

MemcachedProxyFilter::MemcachedProxyFilter(const std::string& stat_prefix, Stats::Scope& scope)
    : stat_prefix_(stat_prefix),
      stats_(generateStats(stat_prefix_, scope)) {
  stats_.downstream_cx_active_.inc();
}

MemcachedProxyFilter::~MemcachedProxyFilter() {
  stats_.downstream_cx_active_.dec();
}

DecoderPtr
MemcachedProxyFilter::createDecoder(DecoderCallbacks& callbacks) {
  return std::make_unique<BinaryProtocolDecoderImpl>(callbacks);
}

FilterStats
MemcachedProxyFilter::generateStats(const std::string& prefix, Stats::Scope& scope) {
  return {
    ALL_FILTER_STATS(POOL_COUNTER_PREFIX(scope, prefix), POOL_GAUGE_PREFIX(scope, prefix))};
}

void
MemcachedProxyFilter::decode(Buffer::Instance& data) {
  if (!binary_protocol_decoder_) {
    binary_protocol_decoder_ = createDecoder(*this);
  }

  try {
    ENVOY_LOG(info, "Attempting to interpret data using the binary protocol");
    binary_protocol_decoder_->onData(data);
  }
  catch (ProtocolError&) {
    // ENVOY_LOG(info, "Attempting to interpret data using the ASCII protocol");
    // if (!ascii_protocol_decoder_) {
    //   ascii_protocol_decoder_ = createDecoder(*this);
    // }
    // ascii_protocol_decoder_->onData(data);
  }

  ASSERT(0 == data.length());
}

void
MemcachedProxyFilter::decodedCommand(Codec::Command op_code) {
  switch (op_code) {
  case Codec::Command::GET:
    stats_.decoded_command_get_.inc();
    break;
  case Codec::Command::SET:
    stats_.decoded_command_set_.inc();
    break;
  case Codec::Command::ADD:
    stats_.decoded_command_add_.inc();
    break;
  case Codec::Command::REPLACE:
    stats_.decoded_command_replace_.inc();
    break;
  case Codec::Command::DELETE:
    stats_.decoded_command_delete_.inc();
    break;
  case Codec::Command::INCREMENT:
    stats_.decoded_command_increment_.inc();
    break;
  case Codec::Command::DECREMENT:
    stats_.decoded_command_decrement_.inc();
    break;
  case Codec::Command::VERSION:
    stats_.decoded_command_version_.inc();
    break;
  case Codec::Command::GET_K:
    stats_.decoded_command_get_k_.inc();
    break;
  case Codec::Command::APPEND:
    stats_.decoded_command_append_.inc();
    break;
  case Codec::Command::PREPEND:
    stats_.decoded_command_prepend_.inc();
    break;
  default:
    ENVOY_LOG(error, "Unhandled command op code {}", static_cast<uint8_t>(op_code));
  }
}

void
MemcachedProxyFilter::decodedResponse() {
  stats_.decoded_response_.inc();
}

void
MemcachedProxyFilter::decodedRequest() {
  stats_.decoded_request_.inc();
}

void
MemcachedProxyFilter::decodedTotalBodyLength(uint32_t bytes) {
  stats_.decoded_total_body_bytes_.add(bytes);
}

void
MemcachedProxyFilter::initializeReadFilterCallbacks(Network::ReadFilterCallbacks& callbacks) {
  read_callbacks_ = &callbacks;
}

Network::FilterStatus
MemcachedProxyFilter::onData(Buffer::Instance& data, bool) {
  read_buffer_.add(data);
  try {
    decode(read_buffer_);
    return Network::FilterStatus::Continue;
  }
  catch (DecoderError& e) {
    onDecoderError(e);
    return Network::FilterStatus::StopIteration;
  }
}

Network::FilterStatus
MemcachedProxyFilter::onNewConnection() {
  return Network::FilterStatus::Continue;
}

void
MemcachedProxyFilter::onDecoderError(DecoderError e) {
  ENVOY_LOG(error, "decoder error: {}", e.what());
  stats_.decoder_error_.inc();
  read_buffer_.drain(read_buffer_.length());
}

} // namespace MemcachedProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
