#include "extensions/filters/network/memcached_proxy/filter.h"

#include "test/mocks/server/mocks.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MemcachedProxy {

class MemcachedProxyFilterTest : public testing::Test {
public:
  MemcachedProxyFilterTest() {
    initializeFilter();
  }
  ~MemcachedProxyFilterTest() {
    delete filter_;
  };

  void initializeFilter() {
    filter_ = new MemcachedProxyFilter(stat_prefix_, stats_);
    // filter_ = std::make_unique<MemcachedProxyFilter>("test.", stats_);
  }

  std::string statName(std::string name) {
    return fmt::format("{}{}", stat_prefix_, name);
  }

  MemcachedProxyFilter* filter_;
  // std::unique_ptr<MemcachedProxyFilter> filter_;
  Stats::IsolatedStoreImpl stats_;
  NiceMock<Network::MockReadFilterCallbacks> read_callbacks_;
  std::string stat_prefix_{"test"};
};

// MemcachedProxyFilter::DecoderCallbacks tests
TEST_F(MemcachedProxyFilterTest, decodedCommandGet) {
  filter_->decodedCommand(Codec::Command::GET);
  EXPECT_EQ(1U, stats_.counter(statName("decoded_command_get")).value());
}

TEST_F(MemcachedProxyFilterTest, decodedCommandSet) {
  filter_->decodedCommand(Codec::Command::SET);
  EXPECT_EQ(1U, stats_.counter(statName("decoded_command_set")).value());
}

TEST_F(MemcachedProxyFilterTest, decodedCommandAdd) {
  filter_->decodedCommand(Codec::Command::ADD);
  EXPECT_EQ(1U, stats_.counter(statName("decoded_command_add")).value());
}

TEST_F(MemcachedProxyFilterTest, decodedCommandReplace) {
  filter_->decodedCommand(Codec::Command::REPLACE);
  EXPECT_EQ(1U, stats_.counter(statName("decoded_command_replace")).value());
}

TEST_F(MemcachedProxyFilterTest, decodedCommandDelete) {
  filter_->decodedCommand(Codec::Command::DELETE);
  EXPECT_EQ(1U, stats_.counter(statName("decoded_command_delete")).value());
}

TEST_F(MemcachedProxyFilterTest, decodedCommandIncrement) {
  filter_->decodedCommand(Codec::Command::INCREMENT);
  EXPECT_EQ(1U, stats_.counter(statName("decoded_command_increment")).value());
}

TEST_F(MemcachedProxyFilterTest, decodedCommandDecrement) {
  filter_->decodedCommand(Codec::Command::DECREMENT);
  EXPECT_EQ(1U, stats_.counter(statName("decoded_command_decrement")).value());
}

TEST_F(MemcachedProxyFilterTest, decodedCommandVersion) {
  filter_->decodedCommand(Codec::Command::VERSION);
  EXPECT_EQ(1U, stats_.counter(statName("decoded_command_version")).value());
}

TEST_F(MemcachedProxyFilterTest, decodedCommandGetK) {
  filter_->decodedCommand(Codec::Command::GET_K);
  EXPECT_EQ(1U, stats_.counter(statName("decoded_command_get_k")).value());
}

TEST_F(MemcachedProxyFilterTest, decodedCommandAppend) {
  filter_->decodedCommand(Codec::Command::APPEND);
  EXPECT_EQ(1U, stats_.counter(statName("decoded_command_append")).value());
}

TEST_F(MemcachedProxyFilterTest, decodedCommandPrepend) {
  filter_->decodedCommand(Codec::Command::PREPEND);
  EXPECT_EQ(1U, stats_.counter(statName("decoded_command_prepend")).value());
}

TEST_F(MemcachedProxyFilterTest, decodedResponse) {
  filter_->decodedResponse();
  EXPECT_EQ(1U, stats_.counter(statName("decoded_response")).value());
}

TEST_F(MemcachedProxyFilterTest, decodedRequest) {
  filter_->decodedRequest();
  EXPECT_EQ(1U, stats_.counter(statName("decoded_request")).value());
}

TEST_F(MemcachedProxyFilterTest, decodedTotalBodyLength) {
  uint32_t body_length = 1024;
  filter_->decodedTotalBodyLength(body_length);
  EXPECT_EQ(1024U, stats_.counter(statName("decoded_total_body_bytes")).value());
}

// Network::ReadFilter tests
TEST_F(MemcachedProxyFilterTest, initializeReadFilterCallbacks) {
  filter_->initializeReadFilterCallbacks(read_callbacks_);
}

TEST_F(MemcachedProxyFilterTest, onData) {
  Buffer::OwnedImpl fake_data;

  EXPECT_EQ(Network::FilterStatus::Continue, filter_->onData(fake_data, false));
}

TEST_F(MemcachedProxyFilterTest, onNewConnection) {
  EXPECT_EQ(Network::FilterStatus::Continue, filter_->onNewConnection());
}

} // namespace MemcachedProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
