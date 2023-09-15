package(default_visibility = [
    "//nth/debug:__subpackages__",
    "//nth/test:__subpackages__",
])

cc_library(
    name = "action",
    hdrs = ["action.h"],
    deps = [
        "//nth/meta:compile_time_string",
    ],
)

cc_library(
    name = "api",
    hdrs = ["api.h"],
    deps = [
        ":concepts",
    ],
)

cc_library(
    name = "concepts",
    hdrs = ["concepts.h"],
    deps = [
        "//nth/meta:compile_time_string",
    ],
)
cc_library(
    name = "formatter",
    hdrs = ["formatter.h"],
    deps = [
        "//nth/strings/format:universal",
    ],
)

cc_library(
    name = "injector",
    hdrs = ["injector.h"],
    deps = [
        ":action",
        ":implementation",
    ],
)

cc_library(
    name = "implementation",
    hdrs = ["implementation.h"],
    deps = [
        ":action",
        ":concepts",
        ":formatter",
        ":traced_expr_base",
        ":traced_value",
        ":traversal_action",
        "//nth/base:attributes",
        "//nth/debug:expectation_result",
        "//nth/meta:type",
        "//nth/io:string_printer",
        "//nth/strings:interpolate",
        "//nth/utility:no_destructor",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "report",
    hdrs = ["report.h"],
    deps = [
        ":traced_expr_base",
    ],
)

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
    deps = [
        ":api",
        ":implementation",
    ],
)

cc_library(
    name = "traced_expr_base",
    hdrs = ["traced_expr_base.h"],
    deps = [
        ":traversal_action",
    ],
)

cc_library(
    name = "traced_value",
    hdrs = ["traced_value.h"],
    deps = [
        ":traced_expr_base",
        "//nth/base:attributes",
    ],
)

cc_library(
    name = "traversal_action",
    hdrs = ["traversal_action.h"],
    srcs = ["traversal_action.cc"],
    deps = [
        ":formatter",
        "//nth/io:string_printer",
    ],
)
