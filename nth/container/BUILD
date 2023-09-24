package(default_visibility = ["//visibility:public"])

cc_library(
    name = "flyweight_map",
    hdrs = ["flyweight_map.h"],
    deps = [
        "//nth/container/internal:index",
        "//nth/debug",
        "//nth/meta:concepts",
        "@com_google_absl//absl/container:flat_hash_set"
    ],
)

cc_test(
    name = "flyweight_map_test",
    srcs = ["flyweight_map_test.cc"],
    deps = [
        ":flyweight_map",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "flyweight_set",
    hdrs = ["flyweight_set.h"],
    deps = [
        "//nth/container/internal:index",
        "//nth/debug",
        "//nth/meta:concepts",
        "@com_google_absl//absl/container:flat_hash_set"
    ],
)

cc_test(
    name = "flyweight_set_test",
    srcs = ["flyweight_set_test.cc"],
    deps = [
        ":flyweight_set",
        "@com_google_googletest//:gtest_main",
    ],
)
