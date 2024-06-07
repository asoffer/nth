package(default_visibility = ["//visibility:public"])

cc_library(
    name = "format",
    hdrs = ["format.h"],
    deps = [
        ":string",
    ],
)

cc_library(
    name = "interpolate",
    hdrs = ["interpolate.h"],
    deps = [
        ":string",
        ":types",
        "//nth/strings/interpolate/internal:interpolate",
        "//nth/io:printer",
        "//nth/base:attributes",
    ],
)

cc_binary(
    name = "interpolate_main",
    srcs = ["interpolate.cc"],
    deps = [
        ":interpolate",
        "//nth/strings/interpolate/internal:interpolate",
        "//nth/io:file_printer",
        "//nth/io:printer",
        "//nth/base:attributes",
    ],
)

cc_library(
    name = "string",
    hdrs = ["string.h"],
    deps = [
        "//nth/meta:compile_time_string",
        "//nth/strings/interpolate/internal:validate",
    ],
)

cc_library(
    name = "types",
    hdrs = ["types.h"],
    deps = [
        ":format",
        ":string",
        "//nth/meta:type",
    ],
)

cc_test(
    name = "types_test",
    srcs = ["types_test.cc"],
    deps = [
        ":types",
        "//nth/meta:type",
    ],
)
