package(default_visibility = ["//visibility:public"])

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
    srcs = [
        "internal/trace.h",
        "internal/trace_declare_api.h",
        "internal/trace_operators.h",
    ],
    deps = [
        "//nth/base:attributes",
        "//nth/meta:type",
        "//nth/io:string_printer",
        "//nth/io:universal_print",
    ],
)

cc_test(
    name = "trace_test",
    srcs = ["trace_test.cc"],
    deps = [
        ":trace",
        "@com_google_googletest//:gtest_main",
    ],
)
