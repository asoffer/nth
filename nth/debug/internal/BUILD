package(default_visibility = ["//nth/debug:__subpackages__"])

cc_library(
    name = "trace",
    hdrs = [
        "trace.h",
        "trace_declare_api.h",
        "trace_operators.h",
    ],
    deps = [
        "//nth/base:attributes",
        "//nth/base:macros",
        "//nth/configuration:verbosity",
        "//nth/debug:source_location",
        "//nth/meta:type",
        "//nth/io:string_printer",
        "//nth/strings:interpolate",
    ],
)
