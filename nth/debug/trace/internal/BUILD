package(default_visibility = ["//nth/debug:__subpackages__"])

cc_library(
    name = "trace",
    hdrs = [
        "trace.h",
        "declare_api.h",
        "operators.h",
    ],
    deps = [
        "//nth/base:attributes",
        "//nth/base:macros",
        "//nth/configuration:verbosity",
        "//nth/debug/log",
        "//nth/debug:source_location",
        "//nth/meta:type",
        "//nth/io:string_printer",
        "//nth/strings:interpolate",
    ],
)
