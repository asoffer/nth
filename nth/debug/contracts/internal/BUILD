package(default_visibility = [
    "//nth/debug:__subpackages__",
    "//nth/test:__subpackages__",
])

cc_library(
    name = "macros",
    hdrs = ["macros.h"],
    deps = [
        "//nth/base:configuration",
        "//nth/base:macros",
    ],
)

cc_library(
    name = "checker",
    hdrs = ["checker.h"],
    deps = [
        "//nth/format",
        "//nth/format/interpolate",
        "//nth/format/interpolate:spec",
        "//nth/format/interpolate:string_view",
        "//nth/io/writer",
        "//nth/meta:type",
    ],
)

cc_library(
    name = "contracts",
    hdrs = ["contracts.h"],
    deps = [
        ":checker",
        ":enabler",
        ":macros",
        ":on_exit",
        ":responder",
        "//nth/debug/log",
        "//nth/debug/trace/internal:trace",
        "//nth/format",
        "//nth/format/interpolate",
        "//nth/io/writer",
    ],
)

cc_library(
    name = "enabler",
    hdrs = ["enabler.h"],
    deps = [
        "//nth/base:section",
        "//nth/debug:source_location",
    ],
)

cc_library(
    name = "on_exit",
    hdrs = ["on_exit.h"],
    deps = [
        "//nth/debug/log",
        "//nth/debug:source_location",
    ],
)

cc_library(
    name = "responder",
    hdrs = ["responder.h"],
    srcs = ["responder.cc"],
    deps = [
        "//nth/configuration:trace",
        "//nth/debug/log",
        "//nth/debug:expectation_result",
        "//nth/debug/property/internal:concepts",
        "//nth/debug/property/internal:implementation",
        "//nth/debug/trace/internal:implementation",
        "@abseil-cpp//absl/synchronization",
    ],
)
