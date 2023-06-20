package(default_visibility = ["//visibility:public"])

cc_library(
    name = "source_location",
    hdrs = ["source_location.h"],
    deps = [],
)

cc_test(
    name = "source_location_test",
    srcs = ["source_location_test.cc"],
    deps = [
        ":source_location",
    ],
)

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
    srcs = [
        "internal/trace.h",
        "internal/trace_declare_api.h",
        "internal/trace_operators.h",
    ],
    deps = [
        "//nth/base:attributes",
        "//nth/meta:type",
        "//nth/io:string_printer",
        "//nth/io:universal_print",
    ],
)

cc_test(
    name = "trace_test",
    srcs = ["trace_test.cc"],
    deps = [
        ":trace",
    ],
)
