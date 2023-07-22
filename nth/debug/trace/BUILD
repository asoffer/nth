package(default_visibility = ["//visibility:public"])

cc_library(
    name = "matcher",
    hdrs = ["matcher.h"],
    deps = [
        ":trace",
    ],
)

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
    srcs = ["trace.cc"],
    deps = [
        "//nth/base:attributes",
        "//nth/base:macros",
        "//nth/configuration:verbosity",
        "//nth/debug:source_location",
        "//nth/debug/trace/internal:trace",
        "//nth/meta:type",
        "//nth/io:string_printer",
        "//nth/strings:interpolate",
    ],
)

cc_test(
    name = "trace_test",
    srcs = ["trace_test.cc"],
    deps = [
        ":trace",
        "//nth/debug/log:stderr_log_sink",
    ],
)
