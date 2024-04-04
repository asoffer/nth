package(default_visibility = ["//visibility:public"])

cc_library(
    name = "compile_time_string",
    hdrs = ["compile_time_string.h"],
    deps = [
        "//nth/meta/concepts:constructible",
    ],
)

cc_test(
    name = "compile_time_string_test",
    srcs = ["compile_time_string_test.cc"],
    deps = [
        ":compile_time_string",
    ],
)

cc_library(
    name = "concepts",
    hdrs = ["concepts.h"],
    deps = [
        "//nth/meta/concepts:convertible",
        "//nth/meta/concepts:core",
    ],
)

cc_test(
    name = "concepts_test",
    srcs = ["concepts_test.cc"],
    deps = [
        ":concepts",
    ],
)

cc_library(
    name = "constant",
    hdrs = ["constant.h"],
    deps = [
        ":type",
    ],
)

cc_test(
    name = "constant_test",
    srcs = ["constant_test.cc"],
    deps = [
        ":constant",
        ":type",
    ],
)

cc_library(
    name = "layout",
    hdrs = ["layout.h"],
    deps = [
        ":sequence",
        ":type",
    ],
)

cc_test(
    name = "layout_test",
    srcs = ["layout_test.cc"],
    deps = [":layout"],
)

cc_library(
    name = "sequence",
    hdrs = ["sequence.h"],
    deps = [
        "//nth/meta/concepts:invocable",
    ],
)

cc_test(
    name = "sequence_test",
    srcs = ["sequence_test.cc"],
    deps = [
        ":type",
        ":sequence",
    ],
)

cc_library(
    name = "stateful",
    hdrs = ["stateful.h"],
    visibility = [
        "//nth/test:__subpackages__",
    ],
    deps = [
        ":constant",
        ":sequence",
        "//nth/base:macros",
    ],
)

cc_test(
    name = "stateful_test",
    srcs = ["stateful_test.cc"],
    deps = [
        ":stateful",
    ],
)

cc_library(
    name = "type",
    hdrs = ["type.h"],
    deps = [
        ":compile_time_string",
        ":sequence"
    ],
)

cc_test(
    name = "type_test",
    srcs = ["type_test.cc"],
    deps = [
        ":type",
    ],
)
