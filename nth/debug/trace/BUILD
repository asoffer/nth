package(default_visibility = ["//visibility:public"])

cc_library(
    name = "expectation_result",
    visibility = ["//nth/debug/trace:__subpackages__"],
    hdrs = ["expectation_result.h"],
    srcs = ["expectation_result.cc"],
    deps = [
        "//nth/debug:source_location",
    ]
)

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
    srcs = ["trace.cc"],
    deps = [
        ":expectation_result",
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
