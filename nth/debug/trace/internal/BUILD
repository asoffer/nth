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
        "//nth/base:attributes",
        "//nth/base:core",
        "//nth/debug/property/internal:concepts",
        "//nth/format",
        "//nth/format/interpolate",
        "//nth/hash:fnv1a",
        "//nth/io/writer",
        "//nth/meta:compile_time_string",
        "//nth/meta/concepts:c_array",
        "//nth/meta/concepts:core",
        "//nth/meta/concepts:invocable",
        "//nth/memory:address",
        "//nth/memory:bytes",
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
        "//nth/base:attributes",
        "//nth/configuration:trace",
        "//nth/io/writer:string",
    ],
)
