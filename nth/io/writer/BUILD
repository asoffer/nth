package(default_visibility = ["//visibility:public"])

cc_library(
    name = "buffered",
    hdrs = ["buffered.h"],
    deps = [":writer"],
)

# TODO: buffered_test

cc_library(
    name = "file",
    hdrs = ["file.h"],
    srcs = ["file.cc"],
    deps = [
        ":writer",
        "//nth/io:file_path",
        "//nth/process/syscall:lseek",
        "//nth/process/syscall:open",
        "//nth/process/syscall:write",
    ],
)

cc_test(
    name = "file_test",
    srcs = ["file_test.cc"],
    deps = [
        ":file",
        ":test",
        "//nth/test:main",
    ]
)

cc_library(
    name = "string",
    hdrs = ["string.h"],
    srcs = ["string.cc"],
    deps = [":writer"],
)

cc_test(
    name = "string_test",
    srcs = ["string_test.cc"],
    deps = [
        ":string",
        ":test",
        "//nth/test:main",
    ],
)

cc_library(
    name = "test",
    testonly = True,
    hdrs = ["test.h"],
    deps = [
        ":writer",
        "//nth/test",
    ],
)

cc_library(
    name = "writer",
    hdrs = ["writer.h"],
    deps = [
        "//nth/memory:bytes",
        "//nth/meta/concepts:comparable",
        "//nth/meta/concepts:core",
    ],
)
