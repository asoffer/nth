package(default_visibility = ["//visibility:public"])

cc_library(
    name = "extend",
    hdrs = ["extend.h"],
    srcs = ["internal/extend.h"],
    deps = [
        "//nth/format",
        "//nth/io/writer",
        "//nth/types:reflect",
    ],
)

cc_test(
    name = "extend_test",
    srcs = ["extend_test.cc"],
    deps = [
        ":extend",
        "//nth/test:main",
    ],
)

cc_library(
    name = "equality",
    hdrs = ["equality.h"],
    deps = [
        ":extend",
        "//nth/types:reflect",
    ],
)

cc_test(
    name = "equality_test",
    srcs = ["equality_test.cc"],
    deps = [
        ":equality",
        ":extend",
        "//nth/test:main",
    ],
)


cc_library(
    name = "format",
    hdrs = ["format.h"],
    srcs = ["internal/format.h"],
    deps = [
        ":extend",
        "//nth/meta:constant",
        "//nth/format",
        "//nth/format:cc",
        "//nth/format:common",
        "//nth/container:stack",
        "//nth/debug:unreachable",
        "//nth/types:reflect",
        "//nth/types:structure",
    ],
)

cc_test(
    name = "format_test",
    srcs = ["format_test.cc"],
    deps = [
        ":format",
        ":extend",
        "//nth/format:json",
        "//nth/test:main",
    ],
)

cc_library(
    name = "order",
    hdrs = ["order.h"],
    deps = [
        ":equality",
        ":extend",
        "//nth/types:reflect",
    ],
)

cc_test(
    name = "order_test",
    srcs = ["order_test.cc"],
    deps = [
        ":extend",
        ":order",
        "//nth/test:main",
    ],
)
