package(default_visibility = ["//visibility:public"])

cc_library(
    name = "interpolate",
    hdrs = ["interpolate.h"],
    deps = [
        ":string",
        ":types",
        "//nth/base:attributes",
        "//nth/strings/interpolate/internal:interpolate",
        "//nth/strings/interpolate/internal:parameter_tree",
        "//nth/io:printer",
    ],
)

cc_test(
    name = "interpolate_test",
    srcs = ["interpolate_test.cc"],
    deps = [
        ":interpolate",
        "//nth/io:string_printer",
        "//nth/test/raw:test",
    ],
)

cc_library(
    name = "string",
    hdrs = ["string.h"],
    deps = [
        "//nth/base:attributes",
        "//nth/meta:compile_time_string",
        "//nth/strings/interpolate/internal:validate",
    ],
)

cc_library(
    name = "types",
    hdrs = ["types.h"],
    deps = [
        ":string",
        "//nth/meta:type",
        "//nth/io/format",
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
