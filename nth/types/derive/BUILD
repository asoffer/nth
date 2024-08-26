package(default_visibility = ["//visibility:public"])

cc_library(
    name = "derive",
    hdrs = ["derive.h"],
    srcs = ["internal/derive.h"],
    deps = [
        "//nth/format",
        "//nth/io/writer",
        "//nth/types:reflect",
    ],
)

cc_test(
    name = "derive_test",
    srcs = ["derive_test.cc"],
    deps = [
        ":derive",
        "//nth/test:main",
    ],
)
