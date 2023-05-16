package(default_visibility = ["//visibility:public"])

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
    deps = [
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
