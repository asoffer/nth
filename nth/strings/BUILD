package(default_visibility = ["//visibility:public"])

cc_library(
    name = "edit_distance",
    hdrs = ["edit_distance.h"],
    srcs = ["edit_distance.cc"],
)

cc_test(
    name = "edit_distance_test",
    srcs = ["edit_distance_test.cc"],
    deps = [
        ":edit_distance",
        "//nth/test:main",
    ],
)

cc_library(
    name = "glob",
    hdrs = ["glob.h"],
    srcs = ["internal/glob.h"],
    deps = [],
)

cc_test(
    name = "glob_test",
    srcs = ["glob_test.cc"],
    deps = [
        ":glob",
    ],
)

cc_library(
    name = "null_terminated_string_view",
    hdrs = ["null_terminated_string_view.h"],
    deps = [
        "//nth/base:attributes",
        "//nth/io/writer",
    ],
)

cc_test(
    name = "null_terminated_string_view_test",
    srcs = ["null_terminated_string_view_test.cc"],
    deps = [
        ":null_terminated_string_view",
        "//nth/test:main",
    ],
)

cc_library(
    name = "text",
    hdrs = ["text.h"],
    srcs = ["text.cc"],
    deps = [
        ":text_encoding",
    ],
)

cc_test(
    name = "text_test",
    srcs = ["text_test.cc"],
    deps = [
        ":text",
        "//nth/test:main",
    ],
)

cc_library(
    name = "text_encoding",
    hdrs = ["text_encoding.h"],
    deps = [],
)
