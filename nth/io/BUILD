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
        ":printer",
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

cc_library(
    name = "file_printer",
    hdrs = ["file_printer.h"],
    srcs = ["file_printer.cc"],
    deps = [
        ":printer",
    ],
)

cc_test(
    name = "file_printer_test",
    srcs = ["file_printer_test.cc"],
    deps = [
        ":file",
        ":file_printer",
        "//nth/test:main",
    ],
)

cc_library(
    name = "printer",
    hdrs = ["printer.h"],
    deps = [
        "//nth/meta/concepts:core",
        "//nth/meta:type",
        "//nth/strings/interpolate:string",
        "//nth/strings/interpolate:types",
        "//nth/io/format",
    ],
)

cc_test(
    name = "printer_test",
    srcs = ["printer_test.cc"],
    deps = [
        ":printer",
        "//nth/test:main",
    ],
)

cc_library(
    name = "string_printer",
    hdrs = ["string_printer.h"],
    deps = [
        "//nth/io:printer",
    ],
)

cc_test(
    name = "string_printer_test",
    srcs = ["string_printer_test.cc"],
    deps = [
        ":string_printer",
        "//nth/test:main",
    ],
)
