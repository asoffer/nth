package(default_visibility = ["//visibility:public"])

cc_library(
    name = "extend",
    hdrs = ["extend.h"],
    deps = [
        "//nth/meta:always_false",
        "//nth/types/extend/internal:extend",
    ],
)

cc_test(
    name = "extend_test",
    srcs = ["extend_test.cc"],
    deps = [
        ":extend",
        "//nth/test:main",
    ],
)
