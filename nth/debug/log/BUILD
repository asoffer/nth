package(default_visibility = ["//visibility:public"])

cc_library(
    name = "entry",
    visibility = ["//nth/debug/log:__subpackages__"],
    hdrs = ["entry.h"],
    srcs = ["entry.cc"],
    deps = [
        ":line",
        ":sink",
        "//nth/configuration:log",
        "//nth/debug/log/internal:component_iterator",
    ],
)

cc_library(
    name = "line",
    visibility = ["//nth/debug/log:__subpackages__"],
    hdrs = ["line.h"],
    srcs = ["line.cc"],
    deps = [
        "//nth/debug/log/internal:interpolation_arguments",
        "//nth/debug/log/internal:voidifier",
        "//nth/debug:source_location",
    ],
)

cc_library(
    name = "log",
    hdrs = ["log.h"],
    deps = [
        ":entry",
        ":line",
        ":sink",
        "//nth/base:macros",
        "//nth/debug/log/internal:log",
        "//nth/debug/verbosity",
    ],
)

cc_test(
    name = "log_test",
    srcs = ["log_test.cc"],
    deps = [
        ":log",
        ":vector_log_sink",
        "//nth/debug:unreachable",
    ],
)

cc_library(
    name = "sink",
    visibility = ["//nth/debug/log:__subpackages__"],
    hdrs = ["sink.h"],
    srcs = ["sink.cc"],
    deps = [
      "//nth/base/internal:global_registry",
    ],
)

cc_library(
    name = "stderr_log_sink",
    hdrs = ["stderr_log_sink.h"],
    deps = [
        ":entry",
        ":line",
        ":sink",
        "//nth/configuration:log",
        "//nth/io:file_printer",
    ],
)

cc_library(
    name = "vector_log_sink",
    hdrs = ["vector_log_sink.h"],
    deps = [
        ":entry",
        ":line",
        ":sink",
    ],
)
