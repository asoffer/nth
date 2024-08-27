package(default_visibility = ["//visibility:public"])

cc_library(
    name = "buffered",
    hdrs = ["buffered.h"],
    deps = [":writer"],
)

cc_library(
    name = "file",
    hdrs = ["file.h"],
    srcs = ["file.cc"],
    deps = [
        ":writer",
        "//nth/memory:buffer",
        "//nth/io:file_path",
    ],
)

cc_test(
    name = "file_test",
    srcs = ["file_test.cc"],
    deps = [
        ":file",
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
        "//nth/memory:bytes",
        "//nth/test:main",
    ],
)

cc_library(
    name = "writer",
    hdrs = ["writer.h"],
    deps = [
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
