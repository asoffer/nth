package(default_visibility = [
    "//nth/debug:__subpackages__",
    "//nth/test:__subpackages__",
])

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
    deps = [
        "//nth/algorithm:tree",
        "//nth/base:attributes",
        "//nth/base:core",
        "//nth/debug/property/internal:concepts",
        "//nth/format/interpolate",
        "//nth/hash:fnv1a",
        "//nth/format",
        "//nth/io/writer",
        "//nth/io/writer:string",
        "//nth/meta:compile_time_string",
        "//nth/meta/concepts:c_array",
        "//nth/meta/concepts:core",
        "//nth/meta/concepts:invocable",
        "//nth/memory:address",
    ],
)
