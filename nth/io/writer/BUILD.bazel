package(default_visibility = ["//visibility:public"])

cc_library(
    name = "file",
    hdrs = ["file.h"],
    srcs = ["file.cc"],
    deps = [
        ":writer",
        "//nth/debug",
        "//nth/memory:buffer",
        "//nth/io:file_path",
    ],
)

cc_test(
    name = "file_test",
    srcs = ["file_test.cc"],
    deps = [
        ":file",
        "//nth/io:file_path",
        "//nth/test:main",
    ]
)

cc_library(
    name = "null",
    hdrs = ["null.h"],
    deps = [
        ":writer",
    ],
)

cc_test(
    name = "null_test",
    srcs = ["null_test.cc"],
    deps = [
        ":null",
        ":writer",
        "//nth/test:main",
    ]
)


cc_library(
    name = "string",
    hdrs = ["string.h"],
    srcs = ["string.cc"],
    deps = [
        ":writer",
        "//nth/base:attributes",
    ],
)

cc_test(
    name = "string_test",
    srcs = ["string_test.cc"],
    deps = [
        ":string",
        ":writer",
        "//nth/memory:bytes",
        "//nth/test:main",
    ],
)

cc_library(
    name = "writer",
    hdrs = ["writer.h"],
    deps = [
        "//nth/io/reader",
        "//nth/meta/concepts:convertible",
        "//nth/meta/concepts:core",
    ],
)

cc_test(
    name = "writer_test",
    srcs = ["writer_test.cc"],
    deps = [
        ":writer",
        "//nth/meta:type",
    ],
)
