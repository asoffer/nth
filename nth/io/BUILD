package(default_visibility = ["//visibility:public"])

cc_library(
    name = "environment_variable",
    hdrs = ["environment_variable.h"],
    srcs = ["environment_variable.cc"],
    deps = [
        "//nth/strings:null_terminated_string_view",
    ],
)

cc_test(
    name = "environment_variable_test",
    srcs = ["environment_variable_test.cc"],
    deps = [
        ":environment_variable",
        "//nth/meta:type",
        "//nth/test:main",
    ],
)

cc_library(
    name = "file",
    hdrs = ["file.h"],
    srcs = ["file.cc"],
    deps = [
        ":file_path",
        "//nth/base:indestructible",
        "//nth/debug/contracts",
    ],
)

cc_library(
    name = "file_path",
    hdrs = ["file_path.h"],
    srcs = ["file_path.cc"],
    deps = [
        "//nth/base:core",
        "//nth/io/format",
        "//nth/io/writer",
        "//nth/debug",
    ],
)

cc_test(
    name = "file_path_test",
    srcs = ["file_path_test.cc"],
    deps = [
        ":file_path",
        "//nth/test:main",
    ],
)
