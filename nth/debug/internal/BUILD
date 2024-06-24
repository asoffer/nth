package(default_visibility = ["//nth/debug:__subpackages__"])

cc_library(
    name = "raw_check",
    hdrs = ["raw_check.h"],
    deps = [],
    visibility = [
        "//nth:__subpackages__",
    ],
)

cc_library(
    name = "unimplemented",
    hdrs = ["unimplemented.h"],
    deps = ["//nth/debug/log"],
)

cc_library(
    name = "unreachable",
    hdrs = ["unreachable.h"],
    deps = [
        "//nth/base:configuration",
        "//nth/base:platform",
        "//nth/debug/log",
    ],
)
