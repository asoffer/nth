package(default_visibility = ["//nth/debug/log:__subpackages__"])

cc_library(
    name = "arguments",
    hdrs = ["arguments.h"],
    deps = [
        ":voidifier",
        "//nth/debug/log:line",
        "//nth/debug/log:entry",
    ],
)

cc_library(
    name = "log",
    hdrs = ["log.h"],
    deps = [
        ":arguments",
        ":voidifier",
        "//nth/base:macros",
        "//nth/base:section",
        "//nth/debug/log:line",
        "//nth/debug/log:entry",
        "//nth/debug:source_location",
    ],
)

cc_library(
    name = "voidifier",
    hdrs = ["voidifier.h"],
    deps = [],
)
