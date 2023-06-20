package(default_visibility = ["//visibility:public"])

cc_library(
    name = "buffer",
    hdrs = ["buffer.h"],
    deps = [],
)

cc_test(
    name = "buffer_test",
    srcs = ["buffer_test.cc"],
    deps = [
        ":buffer",
        "//nth/meta:type",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "iterator_range",
    hdrs = ["iterator_range.h"],
    deps = [],
)


cc_test(
    name = "iterator_range_test",
    srcs = ["iterator_range_test.cc"],
    deps = [
        ":iterator_range",
        "//nth/meta:type",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "lazy",
    hdrs = ["lazy.h"],
    deps = [],
)

cc_test(
    name = "lazy_test",
    srcs = ["lazy_test.cc"],
    deps = [
        ":lazy",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "no_destructor",
    hdrs = ["no_destructor.h"],
    deps = [
      ":buffer",
    ],
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
    name = "projected_span",
    hdrs = ["projected_span.h"],
    deps = [
        "//nth/meta:type",
    ],
)

cc_test(
    name = "projected_span_test",
    srcs = ["projected_span_test.cc"],
    deps = [
        ":projected_span",
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

cc_library(
    name = "registration",
    hdrs = ["registration.h"],
    deps = [
        ":no_destructor",
        "//nth/base:macros",
    ],
)

cc_test(
    name = "registration_test",
    srcs = ["registration_test.cc"],
    deps = [
        ":registration",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "required",
    hdrs = ["required.h"],
    deps = [
        "//nth/meta:type",
    ],
)

cc_test(
    name = "required_test",
    srcs = ["required_test.cc"],
    deps = [
        ":required",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "unconstructible",
    hdrs = ["unconstructible.h"],
    deps = [],
)

cc_test(
    name = "unconstructible_test",
    srcs = ["unconstructible_test.cc"],
    deps = [
        ":unconstructible",
    ],
)
