package(default_visibility = ["//visibility:public"])

cc_library(
    name = "tree",
    hdrs = ["tree.h"],
    deps = [
        "//nth/algorithm/internal:tree",
        "//nth/base:core",
        "//nth/container:stack",
        "//nth/debug:unreachable",
    ],
)

cc_test(
    name = "tree_test",
    srcs = ["tree_test.cc"],
    deps = [
        ":tree",
        "//nth/test/raw:test",
    ],
)
