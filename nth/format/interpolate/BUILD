package(default_visibility = ["//visibility:public"])

cc_library(
    name = "interpolate",
    hdrs = ["interpolate.h"],
    deps = [
        ":string",
        ":string_view",
        "//nth/base:attributes",
        "//nth/base:core",
        "//nth/format",
        "//nth/io/writer",
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
        "//nth/format/interpolate/internal:parameter_range",
    ],
)

cc_library(
    name = "string_view",
    hdrs = ["string_view.h"],
    deps = [
        ":string",
        "//nth/base:attributes",
        "//nth/format/interpolate/internal:parameter_range",
    ],
)
