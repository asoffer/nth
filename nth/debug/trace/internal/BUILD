package(default_visibility = [
    "//nth/debug:__subpackages__",
    "//nth/test:__subpackages__",
])

cc_library(
    name = "implementation",
    hdrs = [
        "implementation.h",
        "declare_api.h",
        "operators.h",
    ],
    deps = [
        "//nth/base:attributes",
        "//nth/debug/contracts/internal:trace_action",
        "//nth/debug/property/internal:concepts",
        "//nth/debug/property/internal:property_formatter",
        "//nth/debug:expectation_result",
        "//nth/meta:type",
        "//nth/io:string_printer",
        "//nth/strings:interpolate",
        "//nth/utility:no_destructor",
        "@com_google_absl//absl/synchronization",
    ],
)
