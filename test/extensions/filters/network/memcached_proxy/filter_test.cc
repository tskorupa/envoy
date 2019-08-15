#include "extensions/filters/network/memcached_proxy/filter.h"

#include "common/buffer/buffer_impl.h"

#include "test/mocks/server/mocks.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MemcachedProxy {

class MemcachedProxyFilterTest : public testing::Test {
public:
  void initialize() {
    filter_ = std::make_unique<MemcachedProxyFilter>(stat_prefix_, scope_);
  }

  std::unique_ptr<MemcachedProxyFilter> filter_;
  Stats::IsolatedStoreImpl scope_;
  std::string stat_prefix_{"test"};
};

TEST_F(MemcachedProxyFilterTest, onData) {
  initialize();

  Buffer::OwnedImpl fake_data;
  bool end_stream = false;

  EXPECT_EQ(Network::FilterStatus::Continue, filter_->onData(fake_data, end_stream));
}

TEST_F(MemcachedProxyFilterTest, onNewConnection) {
  initialize();
  EXPECT_EQ(Network::FilterStatus::Continue, filter_->onNewConnection());
}

TEST_F(MemcachedProxyFilterTest, onWrite) {
  initialize();

  Buffer::OwnedImpl fake_data;
  bool end_stream = false;

  EXPECT_EQ(Network::FilterStatus::Continue, filter_->onWrite(fake_data, end_stream));
}

} // namespace MemcachedProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
