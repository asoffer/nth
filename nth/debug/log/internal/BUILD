package(default_visibility = ["//nth/debug/log:__subpackages__"])

cc_library(
    name = "interpolation_arguments",
    hdrs = ["interpolation_arguments.h"],
    deps = [
        ":voidifier",
        "//nth/base:attributes",
        "//nth/io:string_printer",
    ],
)

cc_library(
    name = "log",
    hdrs = ["log.h"],
    deps = [
        "//nth/base:attributes",
        "//nth/base:macros",
        "//nth/configuration:verbosity",
        "//nth/debug/log/internal:interpolation_arguments",
        "//nth/debug/log:sink",
        "//nth/debug/log:stderr_sink",
        "//nth/debug:source_location",
        "//nth/io:string_printer",
    ],
)

cc_library(
    name = "voidifier",
    hdrs = ["voidifier.h"],
    deps = [],
)
