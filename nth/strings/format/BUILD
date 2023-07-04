package(default_visibility = ["//visibility:public"])

cc_library(
    name = "formatter",
    hdrs = ["formatter.h"],
    deps = [
        "//nth/io:printer",
    ],
)

cc_test(
    name = "formatter_test",
    srcs = ["formatter_test.cc"],
    deps = [
        ":formatter",
        "//nth/io:printer",
    ],
)


cc_library(
    name = "universal",
    hdrs = ["universal.h"],
    deps = [
        "//nth/io:printer",
        "//nth/meta:concepts",
        "//nth/meta:type",
        "//nth/utility:required",
    ],
)

cc_test(
    name = "universal_test",
    srcs = ["universal_test.cc"],
    deps = [
        ":universal",
        "//nth/io:string_printer",
        "@com_google_googletest//:gtest_main",
    ],
)
