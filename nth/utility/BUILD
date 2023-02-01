package(default_visibility = ["//visibility:public"])

cc_library(
    name = "no_destructor",
    hdrs = ["no_destructor.h"],
    deps = [],
)

cc_test(
    name = "no_destructor_test",
    srcs = ["no_destructor_test.cc"],
    deps = [
        ":no_destructor",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "ptr_union",
    hdrs = ["ptr_union.h"],
    deps = [
        "//nth/meta:concepts",
        "//nth/meta:type",
    ],
)

cc_test(
    name = "ptr_union_test",
    srcs = ["ptr_union_test.cc"],
    deps = [
        ":ptr_union",
        "@com_google_googletest//:gtest_main",
    ],
)
