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
        "//nth/io/writer",
        "//nth/strings/interpolate",
    ],
)

cc_library(
    name = "line",
    visibility = ["//nth/debug/log:__subpackages__"],
    hdrs = ["line.h"],
    srcs = ["line.cc"],
    deps = [
        "//nth/debug/log/internal:voidifier",
        "//nth/debug:source_location",
        "//nth/io/format",
        "//nth/io/writer",
        "//nth/strings/interpolate",
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
        "//nth/test/raw:test",
    ],
)

cc_library(
    name = "sink",
    visibility = ["//nth/debug/log:__subpackages__"],
    hdrs = ["sink.h"],
    srcs = ["sink.cc"],
    deps = [
        "//nth/base:indestructible",
        "//nth/registration:registrar",
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
        "//nth/io/writer:file",
        "//nth/memory:bytes",
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
