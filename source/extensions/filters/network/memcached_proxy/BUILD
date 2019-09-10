licenses(["notice"])  # Apache 2

load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
    "envoy_package",
)

envoy_package()

envoy_cc_library(
    name = "binary_protocol_decoder_impl",
    srcs = ["binary_protocol_decoder_impl.cc"],
    hdrs = [
        "decoder.h",
        "binary_protocol_decoder_impl.h",
    ],
    deps = [
        ":codec",
        "//include/envoy/buffer:buffer_interface",
        "//source/common/buffer:buffer_lib",
        "//source/common/common:minimal_logger_lib",
    ],
)

envoy_cc_library(
    name = "codec",
    srcs = [],
    hdrs = ["codec.h"],
    deps = [],
)

envoy_cc_library(
    name = "filter",
    srcs = ["filter.cc"],
    hdrs = ["filter.h"],
    deps = [
        ":binary_protocol_decoder_impl",
        ":codec",
        "//include/envoy/network:filter_interface",
        "//include/envoy/stats:stats_macros",
        "//source/common/common:assert_lib",
        "//source/common/buffer:buffer_lib",
        "//source/common/common:minimal_logger_lib",
        "@envoy_api//envoy/config/filter/network/memcached_proxy/v2:memcached_proxy_cc",
    ],
)

envoy_cc_library(
    name = "config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    deps = [
        ":filter",
        "//include/envoy/registry",
        "//source/extensions/filters/network/common:factory_base_lib",
        "//source/extensions/filters/network:well_known_names",
    ],
)
