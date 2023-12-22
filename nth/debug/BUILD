package(default_visibility = ["//visibility:public"])

cc_library(
    name = "debug",
    hdrs = ["debug.h"],
    deps = [
        ":source_location",
        ":unimplemented",
        ":unreachable",
        "//nth/debug/contracts",
        "//nth/debug/log",
        "//nth/debug/property",
        "//nth/debug/trace",
    ],
)

cc_library(
    name = "expectation_result",
    hdrs = ["expectation_result.h"],
    srcs = ["expectation_result.cc"],
    deps = [
        ":source_location",
        "//nth/base/internal:global_registry",
    ],
)

cc_library(
    name = "source_location",
    hdrs = ["source_location.h"],
    deps = [
        "//nth/utility:unconstructible",
    ],
)

cc_test(
    name = "source_location_test",
    srcs = ["source_location_test.cc"],
    deps = [
        ":source_location",
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

cc_library(
    name = "unreachable",
    hdrs = ["unreachable.h"],
    deps = [
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
    deps = [
        ":unreachable",
        "//nth/debug/log:vector_log_sink",
    ],
)
