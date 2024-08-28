package(default_visibility = ["//visibility:public"])

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
    deps = [
        "//nth/base:attributes",
        "//nth/base:macros",
        "//nth/debug/trace/internal:trace",
        "//nth/meta:compile_time_string",
    ],
)

cc_test(
    name = "trace_test",
    srcs = ["trace_test.cc"],
    deps = [
        ":trace",
        "//nth/memory:address",
        "//nth/test/raw:test",
        "//nth/debug/log",
        "//nth/debug/log:sink",
        "//nth/debug/log:file_log_sink",
    ],
)
