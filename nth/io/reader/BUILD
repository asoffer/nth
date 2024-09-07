package(default_visibility = ["//visibility:public"])

cc_library(
    name = "file",
    hdrs = ["file.h"],
    srcs = ["file.cc"],
    deps = [
        ":reader",
        "//nth/io:file_path",
    ],
)

cc_test(
    name = "file_test",
    srcs = ["file_test.cc"],
    deps = [
        ":file",
        "//nth/io/writer:file",
        "//nth/test:main",
    ]
)

cc_library(
    name = "reader",
    hdrs = ["reader.h"],
    deps = [
        "//nth/memory:bytes",
        "//nth/meta/concepts:comparable",
        "//nth/meta/concepts:core",
    ],
)

cc_library(
    name = "string",
    hdrs = ["string.h"],
    srcs = ["string.cc"],
    deps = [":reader"],
)

cc_test(
    name = "string_test",
    srcs = ["string_test.cc"],
    deps = [
        ":string",
        "//nth/test:main",
    ]
)
