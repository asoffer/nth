package(default_visibility = ["//visibility:public"])

cc_library(
    name = "debug",
    hdrs = [
        "debug.h",
        "//nth/debug/property:property.h",
    ],
    deps = [
        ":source_location",
        ":unimplemented",
        ":unreachable",
        "//nth/debug/contracts",
        "//nth/debug/log",
        "//nth/debug/property",
        "//nth/debug/property/internal:concepts",
        "//nth/debug/property/internal:implementation",
        "//nth/debug/trace",
        "//nth/meta/concepts:invocable",
    ],
)

cc_library(
    name = "fakeable_function",
    hdrs = ["fakeable_function.h"],
    deps = [
        "//nth/base:attributes",
        "//nth/base:macros",
    ],
)

cc_library(
    name = "source_location",
    hdrs = ["source_location.h"],
    deps = [
        "//nth/format",
        "//nth/io/writer",
        "//nth/utility:unconstructible",
    ],
)

cc_test(
    name = "source_location_test",
    srcs = ["source_location_test.cc"],
    deps = [
        ":source_location",
        "//nth/format",
        "//nth/io/writer:string",
    ],
)

cc_library(
    name = "unimplemented",
    hdrs = ["unimplemented.h"],
    deps = [
        "//nth/debug/internal:unimplemented",
    ],
)

cc_test(
    name = "unimplemented_compiles_test",
    srcs = ["unimplemented_compiles_test.cc"],
    deps = [
        ":unimplemented",
    ],
)

cc_test(
    name = "unimplemented_test",
    srcs = ["unimplemented_test.cc"],
    deps = [
        ":unimplemented",
        "//nth/test/raw:test",
    ],
)

cc_library(
    name = "unreachable",
    hdrs = ["unreachable.h"],
    deps = [
        "//nth/debug/log",
        "//nth/debug/internal:unreachable",
    ],
)

cc_test(
    name = "unreachable_compiles_test",
    srcs = ["unreachable_compiles_test.cc"],
    deps = [
        ":unreachable",
    ],
)

cc_test(
    name = "unreachable_test",
    srcs = ["unreachable_test.cc"],
    tags = ["skip-opt"],
    deps = [
        ":unreachable",
        "//nth/test/raw:test",
    ],
)
