package(default_visibility = ["//visibility:public"])

cc_library(
    name = "expectation_result",
    visibility = ["//nth/debug/trace:__subpackages__"],
    hdrs = ["expectation_result.h"],
    srcs = ["expectation_result.cc"],
    deps = [
        "//nth/debug:source_location",
    ]
)

cc_library(
    name = "testing",
    testonly = True,
    hdrs = ["testing.h"],
    deps = [
        "//nth/debug:verbosity",
        "//nth/base:macros",
        "//nth/debug/trace/internal:testing",
    ]
)

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
    srcs = ["trace.cc"],
    deps = [
        "//nth/base:attributes",
        "//nth/base:macros",
        "//nth/debug/trace/internal:implementation",
        "//nth/debug/trace/internal:trace",
        "//nth/meta:compile_time_string",
    ],
)

cc_test(
    name = "trace_test",
    srcs = ["trace_test.cc"],
    # We cannot directly test the aborting behavior.
    copts = ["-DNTH_DEBUG_INTERNAL_NO_ABORT_FOR_TEST"],
    deps = [
        ":testing",
        ":trace",
    ],
)
