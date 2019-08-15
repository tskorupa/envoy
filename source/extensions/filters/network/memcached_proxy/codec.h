#pragma once

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MemcachedProxy {

class Codec {
public:
  enum class Command {
    GET,
    SET,
    ADD,
    REPLACE,
    DELETE,
    INCREMENT,
    DECREMENT,
    VERSION,
    GET_K,
    APPEND,
    PREPEND,
  };
};

} // namespace MemcachedProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
