package(default_visibility = ["//visibility:public"])

cc_library(
    name = "reflect",
    hdrs = ["reflect.h"],
    srcs = ["internal/reflect.h"],
    deps = [
        "//nth/base:core",
    ],
)

cc_test(
    name = "reflect_test",
    srcs = ["reflect_test.cc"],
    deps = [
        ":reflect",
        "//nth/test:main",
    ],
)
