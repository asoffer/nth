package(default_visibility = ["//visibility:public"])

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
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
        ":trace",
        "//nth/debug/internal:raw_check",
    ],
)
