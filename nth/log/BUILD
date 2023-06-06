package(default_visibility = ["//visibility:public"])

cc_library(
    name = "log",
    hdrs = ["log.h"],
    srcs = [
        "internal/entry.h",
        "internal/indicator.h",
    ],
    deps = [
        "//nth/io:printer",
        "//nth/io:universal_print",
        "//nth/strings:format",
    ],
)

cc_test(
    name = "log_test",
    srcs = ["log_test.cc"],
    deps = [
        ":log",
        "@com_google_googletest//:gtest_main",
    ],
)
