package(default_visibility = ["//visibility:public"])

cc_library(
    name = "interval",
    hdrs = ["interval.h"],
    deps = [
      "//nth/debug/trace",
    ],
)

cc_test(
    name = "interval_test",
    srcs = ["interval_test.cc"],
    deps = [
        ":interval",
        "//nth/test:main",
    ],
)

cc_library(
    name = "interval_set",
    hdrs = ["interval_set.h"],
    deps = [
        ":interval",
    ],
)

cc_test(
    name = "interval_set_test",
    srcs = ["interval_set_test.cc"],
    deps = [
        ":interval_set",
        "//nth/test:main",
        "//nth/debug/trace:matcher",
    ],
)
