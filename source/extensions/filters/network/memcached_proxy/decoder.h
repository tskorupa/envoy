#pragma once

#include "envoy/buffer/buffer.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MemcachedProxy {

class Decoder {
public:
  virtual ~Decoder() = default;

  virtual void onData(Buffer::Instance& data) PURE;
  virtual void encodeDecoderError(Buffer::Instance& buffer) PURE;
};

using DecoderPtr = std::unique_ptr<Decoder>;

class DecoderCallbacks {
public:
  virtual ~DecoderCallbacks() = default;

  virtual void decodedCommand(Codec::Command op_code) PURE;
  virtual void decodedResponse() PURE;
  virtual void decodedRequest() PURE;
  virtual void decodedTotalBodyLength(uint32_t bytes) PURE;
};

class DecoderError : public EnvoyException {
public:
  DecoderError(const std::string& error) : EnvoyException(error) {}
};

class ProtocolError : public EnvoyException {
public:
  ProtocolError(const std::string& error) : EnvoyException(error) {}
};

} // namespace MemcachedProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
