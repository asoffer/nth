package(default_visibility = ["//nth/debug/log:__subpackages__"])

cc_library(
    name = "arguments",
    hdrs = ["arguments.h"],
    deps = [
        "//nth/debug/log:line",
        "//nth/debug/log:entry",
        "//nth/meta/concepts:core",
    ],
)

cc_library(
    name = "log",
    hdrs = ["log.h"],
    deps = [
        ":arguments",
        "//nth/base:macros",
        "//nth/base:section",
        "//nth/debug/log:line",
        "//nth/debug/log:entry",
        "//nth/debug:source_location",
    ],
)
