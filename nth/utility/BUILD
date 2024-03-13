package(default_visibility = ["//visibility:public"])

cc_library(
    name = "any_invocable",
    hdrs = ["any_invocable.h"],
    deps = [],
)

cc_library(
    name = "bytes",
    hdrs = ["bytes.h"],
    deps = [
        "//nth/base:attributes",
    ],
)

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
        "//nth/test:main",
    ],
)

cc_library(
    name = "early_exit",
    hdrs = ["early_exit.h"],
    deps = [
        ":unconstructible",
        "//nth/base:attributes",
        "//nth/meta:concepts",
        "//nth/meta:type",
    ],
)

cc_library(
    name = "early_exit_test_cases",
    hdrs = ["early_exit_test_cases.h"],
    testonly = True,
    deps = [
        ":early_exit",
        "//nth/test",
    ],
)

cc_test(
    name = "early_exit_test",
    srcs = ["early_exit_test.cc"],
    deps = [
        ":early_exit",
        ":early_exit_test_cases",
        "//nth/meta:type",
        "//nth/test:main",
    ],
)

cc_library(
    name = "function_ref",
    hdrs = ["function_ref.h"],
    deps = [],
)

cc_library(
    name = "iterator_range",
    hdrs = ["iterator_range.h"],
    deps = [
        "//nth/debug",
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
    deps = [],
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
    name = "move",
    hdrs = ["move.h"],
    deps = [],
)

cc_test(
    name = "move_test",
    srcs = ["move_test.cc"],
    deps = [
        ":move",
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
        "//nth/test:main",
    ],
)

cc_library(
    name = "offset_ptr",
    hdrs = ["offset_ptr.h"],
    deps = [],
)

cc_test(
    name = "offset_ptr_test",
    srcs = ["offset_ptr_test.cc"],
    deps = [
        ":offset_ptr",
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
        "//nth/meta:concepts",
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
    name = "registration",
    hdrs = ["registration.h"],
    deps = [],
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
