#include "extensions/filters/network/memcached_proxy/config.h"

#include "common/common/fmt.h"

#include "envoy/registry/registry.h"

#include "extensions/filters/network/memcached_proxy/filter.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MemcachedProxy {

Network::FilterFactoryCb
MemcachedProxyFilterConfigFactory::createFilterFactoryFromProtoTyped(
    const envoy::config::filter::network::memcached_proxy::v2::MemcachedProxy& proto_config,
    Server::Configuration::FactoryContext& context) {

  ASSERT(!proto_config.stat_prefix().empty());
  const std::string stat_prefix = fmt::format("memcached.{}.", proto_config.stat_prefix());

  return [stat_prefix, &context](Network::FilterManager& filter_manager) -> void {
    filter_manager.addReadFilter(std::make_shared<MemcachedProxyFilter>(
      stat_prefix, context.scope()));
  };
}

/**
 * Static registration for the memcached proxy filter. @see RegisterFactory.
 */
REGISTER_FACTORY(MemcachedProxyFilterConfigFactory,
                 Server::Configuration::NamedNetworkFilterConfigFactory);

} // namespace MemcachedProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
