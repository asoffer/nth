package(default_visibility = [
    "//nth/debug:__subpackages__",
    "//nth/test:__subpackages__",
])

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
