package(default_visibility = ["//nth/debug/log:__subpackages__"])

cc_library(
    name = "component_iterator",
    hdrs = ["component_iterator.h"],
    srcs = ["component_iterator.cc"],
    deps = [],
)

cc_library(
    name = "interpolation_arguments",
    hdrs = ["interpolation_arguments.h"],
    deps = [
        ":voidifier",
        "//nth/base:attributes",
        "//nth/configuration:log",
        "//nth/io:string_printer",
        "//nth/strings/interpolate",
    ],
)

cc_library(
    name = "log",
    hdrs = ["log.h"],
    deps = [
        ":interpolation_arguments",
        ":voidifier",
        "//nth/base:macros",
        "//nth/configuration:verbosity",
        "//nth/debug/log:line",
        "//nth/debug/log:entry",
        "//nth/debug:source_location",
        "//nth/strings:interpolate",
    ],
)

cc_library(
    name = "voidifier",
    hdrs = ["voidifier.h"],
    deps = [],
)
