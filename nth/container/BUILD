package(default_visibility = ["//visibility:public"])

cc_library(
    name = "black_hole",
    hdrs = ["black_hole.h"],
    deps = [
        ":flyweight_map",
        "//nth/debug",
    ],
)

cc_test(
    name = "black_hole_test",
    srcs = ["black_hole_test.cc"],
    deps = [
        ":black_hole",
        "//nth/test:main",
    ],
)

cc_library(
    name = "disjoint_set",
    hdrs = ["disjoint_set.h"],
    deps = [
        ":flyweight_map",
        "//nth/debug",
        "//nth/memory:buffer",
    ],
)

cc_test(
    name = "disjoint_set_test",
    srcs = ["disjoint_set_test.cc"],
    deps = [
        ":disjoint_set",
        "//nth/test:main",
    ],
)

cc_library(
    name = "flat_forest",
    hdrs = ["flat_forest.h"],
    deps = [
        "//nth/utility:iterator_range",
    ],
)

cc_test(
    name = "flat_forest_test",
    srcs = ["flat_forest_test.cc"],
    deps = [
        ":flat_forest",
        "//nth/test:main",
    ],
)

cc_library(
    name = "flyweight_map",
    hdrs = ["flyweight_map.h"],
    deps = [
        "//nth/container/internal:index",
        "//nth/debug",
        "//nth/meta/concepts:hash",
        "@abseil-cpp//absl/container:flat_hash_set"
    ],
)

cc_test(
    name = "flyweight_map_test",
    srcs = ["flyweight_map_test.cc"],
    deps = [
        ":flyweight_map",
        "//nth/test:main",
    ],
)

cc_library(
    name = "flyweight_set",
    hdrs = ["flyweight_set.h"],
    deps = [
        "//nth/container/internal:index",
        "//nth/debug",
        "//nth/meta/concepts:hash",
        "@abseil-cpp//absl/container:flat_hash_set"
    ],
)

cc_test(
    name = "flyweight_set_test",
    srcs = ["flyweight_set_test.cc"],
    deps = [
        ":flyweight_set",
        "//nth/test:main",
    ],
)

cc_library(
    name = "free_functions",
    hdrs = ["free_functions.h"],
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
        "@abseil-cpp//absl/container:btree"
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
    name = "stable_container",
    hdrs = ["stable_container.h"],
    deps = [
        ":stack",
        "//nth/debug",
    ],
)

cc_test(
    name = "stable_container_test",
    srcs = ["stable_container_test.cc"],
    deps = [
        ":stable_container",
        "//nth/test:main",
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
