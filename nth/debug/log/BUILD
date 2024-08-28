package(default_visibility = ["//visibility:public"])

cc_library(
    name = "configuration",
    hdrs = ["configuration.h"],
    deps = [
        "//nth/debug:source_location",
    ],
)

cc_library(
    name = "entry",
    visibility = ["//nth/debug/log:__subpackages__"],
    hdrs = ["entry.h"],
    srcs = ["entry.cc"],
    deps = [
        ":line",
        "//nth/format/interpolate",
        "//nth/io/writer",
    ],
)

cc_library(
    name = "line",
    hdrs = ["line.h"],
    deps = [
        "//nth/base:section",
        "//nth/debug:source_location",
        "//nth/format/interpolate:string_view",
    ],
)

cc_library(
    name = "log",
    hdrs = ["log.h"],
    srcs = ["log.cc"],
    deps = [
        ":entry",
        ":line",
        ":sink",
        "//nth/base:macros",
        "//nth/base:section",
        "//nth/debug/log/internal:log",
        "//nth/strings:glob",
    ],
)

cc_test(
    name = "log_test",
    srcs = ["log_test.cc"],
    deps = [
        ":log",
        ":vector_log_sink",
        ":stderr_log_sink",
        "//nth/test/raw:test",
    ],
)

cc_library(
    name = "sink",
    hdrs = ["sink.h"],
    srcs = ["sink.cc"],
    deps = [
        ":configuration",
        ":entry",
        ":line",
        "//nth/base:indestructible",
        "//nth/registration:registrar",
    ],
)

cc_library(
    name = "stderr_log_sink",
    hdrs = ["stderr_log_sink.h"],
    deps = [
        ":configuration",
        ":entry",
        ":line",
        ":sink",
        "//nth/io/writer:file",
        "//nth/memory:bytes",
    ],
)

cc_library(
    name = "vector_log_sink",
    hdrs = ["vector_log_sink.h"],
    deps = [
        ":configuration",
        ":entry",
        ":line",
        ":sink",
    ],
)
