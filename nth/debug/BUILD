package(default_visibility = ["//visibility:public"])

cc_library(
    name = "debug",
    hdrs = ["debug.h"],
    deps = [
        "//nth/debug/trace",
        "//nth/debug/log",
        "//nth/debug/property",
        "//nth/debug:source_location",
        "//nth/debug:unimplemented",
        "//nth/debug:unreachable",
    ],
)

cc_library(
    name = "expectation_result",
    hdrs = ["expectation_result.h"],
    srcs = ["expectation_result.cc"],
    deps = [
        ":source_location",
        "//nth/utility:no_destructor",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_test(
    name = "expectation_result_test",
    srcs = ["expectation_result_test.cc"],
    deps = [
        ":expectation_result",
        "//nth/debug/internal:raw_check",
    ],
)

cc_library(
    name = "source_location",
    hdrs = ["source_location.h"],
    deps = ["//nth/utility:unconstructible"],
)

cc_test(
    name = "source_location_test",
    srcs = ["source_location_test.cc"],
    deps = [":source_location"],
)

cc_library(
    name = "unimplemented",
    hdrs = ["unimplemented.h"],
    deps = ["//nth/debug/internal:unimplemented"],
)

cc_test(
    name = "unimplemented_compiles_test",
    srcs = ["unimplemented_compiles_test.cc"],
    deps = [":unimplemented"],
)

cc_library(
    name = "unreachable",
    hdrs = ["unreachable.h"],
    deps = ["//nth/debug/internal:unreachable"],
)

cc_test(
    name = "unreachable_compiles_test",
    srcs = ["unreachable_compiles_test.cc"],
    deps = [":unreachable"],
)

cc_test(
    name = "unreachable_test",
    srcs = ["unreachable_test.cc"],
    deps = [
        ":unreachable",
        "//nth/debug/log:vector_log_sink",
    ],
)
