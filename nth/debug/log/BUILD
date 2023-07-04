package(default_visibility = ["//visibility:public"])

cc_library(
    name = "entry",
    visibility = ["//visibility:private"],
    hdrs = ["entry.h"],
    deps = [],
)

cc_library(
    name = "line",
    visibility = ["//visibility:private"],
    hdrs = ["line.h"],
    srcs = ["line.cc"],
    deps = [
        ":entry",
        "//nth/base:attributes",
        "//nth/configuration:verbosity",
        "//nth/debug:source_location",
        "//nth/debug/log/internal:interpolation_arguments",
        "//nth/debug/log/internal:voidifier",
    ],
)

cc_library(
    name = "log",
    hdrs = ["log.h"],
    deps = [
        "//nth/base:macros",
        "//nth/debug/log/internal:log",
        "//nth/debug:verbosity",
    ],
)

cc_test(
    name = "log_test",
    srcs = ["log_test.cc"],
    deps = [
        ":log",
    ],
)

cc_library(
    name = "sink",
    hdrs = ["sink.h"],
    deps = [
        ":entry",
        ":line",
    ],
)

cc_library(
    name = "stderr_sink",
    hdrs = ["stderr_sink.h"],
    deps = [
        ":entry",
        ":line",
        ":sink",
        "//nth/io:file_printer",
        "//nth/strings:interpolate",
    ],
)
