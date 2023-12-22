package(default_visibility = ["//nth/debug:__subpackages__"])

cc_library(
    name = "raw_check",
    hdrs = ["raw_check.h"],
    deps = [],
    visibility = [
        "//nth/debug:__subpackages__",
        "//nth/base/internal:__pkg__",
    ],
)

cc_library(
    name = "unimplemented",
    hdrs = ["unimplemented.h"],
    deps = ["//nth/debug/log"],
)

cc_library(
    name = "unreachable",
    hdrs = ["unreachable.h"],
    deps = [
        "//nth/base:platform",
        "//nth/debug/log",
    ],
)

cc_library(
    name = "verbosity",
    hdrs = ["verbosity.h"],
    visibility = [
        "//nth/debug:__subpackages__",
        "//nth/configuration:__subpackages__",
    ],
    deps = [
        "//nth/base:attributes",
        "//nth/base:configuration",
        "//nth/debug:source_location",
    ],
)

cc_test(
    name = "verbosity_fast_test",
    copts = ["-DNTH_COMMANDLINE_BUILD_MODE=fast"],
    srcs = ["verbosity_test.cc"],
    deps = [":verbosity"],
)

cc_test(
    name = "verbosity_harden_test",
    copts = ["-DNTH_COMMANDLINE_BUILD_MODE=harden"],
    srcs = ["verbosity_test.cc"],
    deps = [":verbosity"],
)

cc_test(
    name = "verbosity_optimize_test",
    copts = ["-DNTH_COMMANDLINE_BUILD_MODE=optimize"],
    srcs = ["verbosity_test.cc"],
    deps = [":verbosity"],
)

cc_test(
    name = "verbosity_debug_test",
    copts = ["-DNTH_COMMANDLINE_BUILD_MODE=debug"],
    srcs = ["verbosity_test.cc"],
    deps = [":verbosity"],
)
