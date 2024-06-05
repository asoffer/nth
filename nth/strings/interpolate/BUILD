package(default_visibility = ["//visibility:public"])

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
        "//nth/meta:type",
        ":string",
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
