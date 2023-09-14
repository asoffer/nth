package(default_visibility = [
    "//nth/debug:__subpackages__",
    "//nth/test:__subpackages__",
])

cc_library(
    name = "contracts",
    hdrs = ["contracts.h"],
    deps = [
        ":on_exit",
        ":responder",
        ":trace_injector",
        "//nth/base:macros",
    ],
)

cc_library(
    name = "on_exit",
    hdrs = ["on_exit.h"],
    deps = [
        "//nth/debug:source_location",
    ],
)

cc_library(
    name = "responder",
    hdrs = ["responder.h"],
    srcs = ["responder.cc"],
    deps = [
        "//nth/debug/log",
        "//nth/debug:expectation_result",
        "//nth/debug/trace/internal:implementation",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "trace_injector",
    hdrs = ["trace_injector.h"],
    deps = [
        "//nth/debug/trace/internal:implementation",
    ],
)

cc_library(
    name = "trace_action",
    hdrs = ["trace_action.h"],
    deps = [
        "//nth/meta:compile_time_string",
    ],
)
