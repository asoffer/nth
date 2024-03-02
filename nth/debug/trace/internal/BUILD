package(default_visibility = [
    "//nth/debug:__subpackages__",
    "//nth/test:__subpackages__",
])

cc_library(
    name = "actions",
    hdrs = ["actions.h"],
    deps = [
        ":traced_value",
        "//nth/meta:compile_time_string",
    ],
)

cc_library(
    name = "api",
    hdrs = ["api.h"],
    deps = [
        ":implementation",
        "//nth/hash:fnv1a",
    ],
)

cc_library(
    name = "injector",
    hdrs = ["injector.h"],
    deps = [
        ":actions",
        ":implementation",
        "//nth/meta/concepts:c_array",
    ],
)

cc_library(
    name = "implementation",
    hdrs = ["implementation.h"],
    deps = [
        ":actions",
        ":traced_expr_base",
        ":traced_value",
        ":traversal_action",
        "//nth/base:attributes",
        "//nth/debug:expectation_result",
        "//nth/meta:type",
    ],
)

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
    deps = [
        ":actions",
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
        "//nth/io:string_printer",
    ],
)

cc_library(
    name = "traversal_action",
    hdrs = ["traversal_action.h"],
    srcs = ["traversal_action.cc"],
    deps = [
        "//nth/configuration:trace",
        "//nth/io:string_printer",
    ],
)
