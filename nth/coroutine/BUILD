package(default_visibility = ["//visibility:public"])

cc_library(
    name = "coroutine",
    hdrs = ["coroutine.h"],
    deps = [
        "//nth/meta:type",
        "//nth/utility:buffer",
    ],
)

cc_test(
    name = "coroutine_test",
    srcs = ["coroutine_test.cc"],
    deps = [
        ":coroutine",
        "//nth/meta:type",
        "//nth/utility:buffer",
        "@com_google_googletest//:gtest_main",
    ],
)
