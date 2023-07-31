package(default_visibility = ["//visibility:public"])

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
    hdrs = [
        "unimplemented.h",
        "internal/unimplemented.h",
    ],
    deps = [
        "//nth/debug/log",
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
    hdrs = [
        "unreachable.h",
        "internal/unreachable.h",
    ],
    deps = [
        "//nth/base:platform",
        "//nth/debug/log",
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

cc_library(
    name = "verbosity",
    hdrs = ["verbosity.h"],
    deps = [
        ":source_location",
        "//nth/base:attributes",
        "//nth/base:configuration",
    ],
)

cc_test(
    name = "verbosity_fast_test",
    copts = ["-DNTH_COMMANDLINE_BUILD_MODE=fast"],
    srcs = ["verbosity_test.cc"],
    deps = [
        ":verbosity",
    ],
)

cc_test(
    name = "verbosity_harden_test",
    copts = ["-DNTH_COMMANDLINE_BUILD_MODE=harden"],
    srcs = ["verbosity_test.cc"],
    deps = [
        ":verbosity",
    ],
)

cc_test(
    name = "verbosity_optimize_test",
    copts = ["-DNTH_COMMANDLINE_BUILD_MODE=optimize"],
    srcs = ["verbosity_test.cc"],
    deps = [
        ":verbosity",
    ],
)

cc_test(
    name = "verbosity_debug_test",
    copts = ["-DNTH_COMMANDLINE_BUILD_MODE=debug"],
    srcs = ["verbosity_test.cc"],
    deps = [
        ":verbosity",
    ],
)
