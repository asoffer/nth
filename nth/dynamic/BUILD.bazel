package(default_visibility = ["//visibility:public"])

cc_library(
    name = "jit_function",
    hdrs = ["jit_function.h"],
    deps = [
        "//nth/dynamic/internal:jit_function_base",
    ],
)

cc_test(
    name = "jit_function_test",
    srcs = ["jit_function_test.cc"],
    deps = [
        ":jit_function",
        "//nth/base:platform",
        "//nth/test:main",
    ],
)
