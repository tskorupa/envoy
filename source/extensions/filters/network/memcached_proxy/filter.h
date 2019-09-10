#pragma once

#include "extensions/filters/network/memcached_proxy/binary_protocol_decoder_impl.h"
#include "extensions/filters/network/memcached_proxy/codec.h"

#include "common/buffer/buffer_impl.h"

#include "envoy/network/filter.h"
#include "envoy/stats/scope.h"
#include "envoy/stats/stats_macros.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MemcachedProxy {

#define ALL_FILTER_STATS(COUNTER, GAUGE)                      \
  COUNTER(decoded_response)                                   \
  COUNTER(decoded_request)                                    \
  COUNTER(decoded_command_get)                                \
  COUNTER(decoded_command_set)                                \
  COUNTER(decoded_command_add)                                \
  COUNTER(decoded_command_replace)                            \
  COUNTER(decoded_command_delete)                             \
  COUNTER(decoded_command_increment)                          \
  COUNTER(decoded_command_decrement)                          \
  COUNTER(decoded_command_version)                            \
  COUNTER(decoded_command_get_k)                              \
  COUNTER(decoded_command_append)                             \
  COUNTER(decoded_command_prepend)                            \
  COUNTER(decoder_error)                                      \
  COUNTER(decoded_total_body_bytes)                           \
  GAUGE(downstream_cx_active, Accumulate)

struct FilterStats {
  ALL_FILTER_STATS(GENERATE_COUNTER_STRUCT, GENERATE_GAUGE_STRUCT)
};

class MemcachedProxyFilter : public Network::ReadFilter,
                             public DecoderCallbacks,
                             Logger::Loggable<Logger::Id::memcached> {
public:
  MemcachedProxyFilter(const std::string& stat_prefix, Stats::Scope& scope);
  ~MemcachedProxyFilter() override;

  // MemcachedProxyFilter::DecoderCallbacks
  void decodedCommand(Codec::Command op_code) override;
  void decodedResponse() override;
  void decodedRequest() override;
  void decodedTotalBodyLength(uint32_t bytes) override;

  // Network::ReadFilter
  void initializeReadFilterCallbacks(Network::ReadFilterCallbacks& callbacks) override;
  Network::FilterStatus onData(Buffer::Instance& data, bool end_stream) override;
  Network::FilterStatus onNewConnection() override;

private:
  DecoderPtr createDecoder(DecoderCallbacks& callbacks);
  void decode(Buffer::Instance& data);
  static FilterStats generateStats(const std::string& prefix, Stats::Scope& scope);
  void onDecoderError(DecoderError e);

  DecoderPtr binary_protocol_decoder_;
  Buffer::OwnedImpl read_buffer_;
  Network::ReadFilterCallbacks* read_callbacks_{};
  std::string stat_prefix_;
  FilterStats stats_;
};

} // namespace MemcachedProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
