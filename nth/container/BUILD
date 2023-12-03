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

cc_library(
    name = "interval",
    hdrs = ["interval.h"],
    deps = [
      "//nth/debug",
    ],
)

cc_test(
    name = "interval_test",
    srcs = ["interval_test.cc"],
    deps = [
        ":interval",
        "//nth/test:main",
    ],
)

cc_library(
    name = "interval_set",
    hdrs = ["interval_set.h"],
    deps = [
        ":interval",
        "//nth/debug",
    ],
)

cc_test(
    name = "interval_set_test",
    srcs = ["interval_set_test.cc"],
    deps = [
        ":interval_set",
        "//nth/test:main",
        "//nth/debug/property",
    ],
)

cc_library(
    name = "interval_map",
    hdrs = ["interval_map.h"],
    deps = [
        ":interval",
        ":interval_set",
        "//nth/debug",
        "@com_google_absl//absl/container:btree"
    ],
)

cc_test(
    name = "interval_map_test",
    srcs = ["interval_map_test.cc"],
    deps = [
        ":interval_map",
        "//nth/test:main",
        "//nth/debug/property",
    ],
)

cc_library(
    name = "stack",
    hdrs = ["stack.h"],
    deps = [
        "//nth/debug",
    ],
)

cc_test(
    name = "stack_test",
    srcs = ["stack_test.cc"],
    deps = [
        ":stack",
        "//nth/test:main",
    ],
)
