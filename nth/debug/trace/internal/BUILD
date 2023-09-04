package(default_visibility = ["//nth/debug:__subpackages__"])

cc_library(
    name = "trace",
    hdrs = [
        "trace.h",
        "declare_api.h",
        "operators.h",
    ],
    srcs = [
        "trace.cc",
    ],
    deps = [
        "//nth/base:attributes",
        "//nth/base:macros",
        "//nth/configuration:verbosity",
        "//nth/debug/log",
        "//nth/debug/property/internal:concepts",
        "//nth/debug/property/internal:property_formatter",
        "//nth/debug:source_location",
        "//nth/debug/trace:expectation_result",
        "//nth/meta:type",
        "//nth/io:string_printer",
        "//nth/strings:interpolate",
        "//nth/utility:no_destructor",
        "@com_google_absl//absl/synchronization",
    ],
)
