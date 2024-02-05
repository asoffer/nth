package(default_visibility = ["//visibility:public"])

cc_library(
    name = "deserialize",
    hdrs = ["deserialize.h"],
    deps = [
        ":reader",
        "//nth/container:free_functions",
        "//nth/utility:bytes",
    ],
)

cc_library(
    name = "environment_variable",
    hdrs = ["environment_variable.h"],
    srcs = ["environment_variable.cc"],
    deps = [
        "//nth/strings:null_termination",
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
        "//nth/debug/contracts",
        "//nth/utility:no_destructor",
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
    deps = [],
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
    deps = [],
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
    name = "reader",
    hdrs = ["reader.h"],
    deps = [],
)

cc_library(
    name = "reader_test",
    testonly = True,
    hdrs = ["reader_test.h"],
    deps = [
        ":reader",
        "//nth/test",
    ],
)

cc_test(
    name = "round_trip_test",
    srcs = ["round_trip_test.cc"],
    deps = [
        ":deserialize",
        ":string_reader",
        ":string_writer",
        ":serialize",
        "//nth/container:flyweight_set",
        "//nth/test:main",
    ],
)

cc_library(
    name = "serialize",
    hdrs = ["serialize.h"],
    deps = [
        ":writer",
        "//nth/container:free_functions",
        "//nth/utility:bytes",
    ],
)

cc_library(
    name = "string_printer",
    hdrs = ["string_printer.h"],
    deps = [],
)

cc_test(
    name = "string_printer_test",
    srcs = ["string_printer_test.cc"],
    deps = [
        ":string_printer",
        "//nth/test:main",
    ],
)

cc_library(
    name = "string_reader",
    hdrs = ["string_reader.h"],
    srcs = ["string_reader.cc"],
    deps = [":reader"],
)

cc_test(
    name = "string_reader_test",
    srcs = ["string_reader_test.cc"],
    deps = [
        ":reader_test",
        ":string_reader",
        "//nth/test:main",
    ],
)

cc_library(
    name = "string_writer",
    hdrs = ["string_writer.h"],
    srcs = ["string_writer.cc"],
    deps = [":writer"],
)

cc_test(
    name = "string_writer_test",
    srcs = ["string_writer_test.cc"],
    deps = [
        ":writer_test",
        ":string_writer",
        "//nth/test:main",
    ],
)

cc_library(
    name = "writer",
    hdrs = ["writer.h"],
    deps = [
        "//nth/utility:bytes",
    ],
)

cc_library(
    name = "writer_test",
    testonly = True,
    hdrs = ["writer_test.h"],
    deps = [
        ":writer",
        "//nth/test",
    ],
)
