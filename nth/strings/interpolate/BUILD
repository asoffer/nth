package(default_visibility = ["//visibility:public"])

cc_library(
    name = "interpolate",
    hdrs = ["interpolate.h"],
    deps = [
        ":string",
        ":types",
        "//nth/base:attributes",
        "//nth/base:core",
        "//nth/io/format:interpolation_spec",
        "//nth/io/writer",
        "//nth/memory:bytes",
        "//nth/strings/interpolate/internal:interpolate",
    ],
)

cc_test(
    name = "interpolate_test",
    srcs = ["interpolate_test.cc"],
    deps = [
        ":interpolate",
        "//nth/io/writer:string",
        "//nth/test/raw:test",
    ],
)

cc_library(
    name = "string",
    hdrs = ["string.h"],
    deps = [
        "//nth/base:attributes",
        "//nth/meta:compile_time_string",
        "//nth/strings/interpolate/internal:parameter_range",
        "//nth/strings/interpolate/internal:validate",
    ],
)

cc_library(
    name = "types",
    hdrs = ["types.h"],
    deps = [
        ":string",
        "//nth/io/format",
        "//nth/meta:type",
        "//nth/meta/concepts:convertible",
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
