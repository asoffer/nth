package(default_visibility = ["//visibility:public"])

cc_library(
    name = "attributes",
    hdrs = ["attributes.h"],
    deps = [
        ":macros",
    ],
)

cc_test(
    name = "attributes_test",
    srcs = ["attributes_test.cc"],
    deps = [":attributes"],
)

cc_library(
    name = "configuration",
    hdrs = ["configuration.h"],
    deps = [
        ":macros",
    ],
)

cc_library(
    name = "macros",
    hdrs = ["macros.h"],
    deps = [
        ":platform",
    ],
)

cc_test(
    name = "macros_test",
    srcs = ["macros_test.cc"],
    deps = [
        ":macros"
    ],
)

cc_library(
    name = "pack",
    hdrs = ["pack.h"],
    deps = [
        ":macros",
    ],
)

cc_test(
    name = "pack_test",
    srcs = ["pack_test.cc"],
    deps = [
        ":pack"
    ],
)

cc_library(
    name = "platform",
    hdrs = ["platform.h"],
    deps = [],
)
