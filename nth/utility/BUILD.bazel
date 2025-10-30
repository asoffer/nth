package(default_visibility = ["//visibility:public"])

cc_library(
    name = "any_invocable",
    hdrs = ["any_invocable.h"],
    deps = [
        "//nth/base:core",
        "//nth/meta/concepts:core",
        "//nth/meta/concepts:invocable",
    ],
)

cc_library(
    name = "function_ref",
    hdrs = ["function_ref.h"],
    deps = [
        "//nth/memory:address",
        "//nth/meta/concepts:core",
        "//nth/meta/concepts:invocable",
    ],
)

cc_library(
    name = "iterator_range",
    hdrs = ["iterator_range.h"],
    deps = [
        "//nth/base:core",
        "//nth/debug",
        "//nth/meta/concepts:core",
    ],
)


cc_test(
    name = "iterator_range_test",
    srcs = ["iterator_range_test.cc"],
    deps = [
        ":iterator_range",
        "//nth/meta:type",
        "//nth/test:main",
    ],
)

cc_library(
    name = "lazy",
    hdrs = ["lazy.h"],
    deps = [
        "//nth/base:core",
        "//nth/meta/concepts:invocable",
    ],
)

cc_test(
    name = "lazy_test",
    srcs = ["lazy_test.cc"],
    deps = [
        ":lazy",
        "//nth/test:main",
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
        "//nth/test:main",
    ],
)

cc_library(
    name = "ptr_union",
    hdrs = ["ptr_union.h"],
    deps = [
        "//nth/meta/concepts:core",
        "//nth/meta:type",
    ],
)

cc_test(
    name = "ptr_union_test",
    srcs = ["ptr_union_test.cc"],
    deps = [
        ":ptr_union",
        "//nth/test:main",
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
        "//nth/test:main",
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
