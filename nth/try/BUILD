package(default_visibility = ["//visibility:public"])

cc_library(
    name = "try",
    hdrs = [
        "try.h",
        "internal/try.h",
    ],
    deps = [
        "//nth/base:attributes",
        "//nth/base:core",
        "//nth/meta:type",
        "//nth/meta/concepts:core",
    ],
)

cc_test(
    name = "try_test",
    srcs = ["try_test.cc"],
    deps = [
        ":try",
        "//nth/test:main",
    ],
)
