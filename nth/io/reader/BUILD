package(default_visibility = ["//visibility:public"])

cc_library(
    name = "file",
    hdrs = ["file.h"],
    srcs = ["file.cc"],
    deps = [
        ":reader",
        "//nth/io:file_path",
        "//nth/process/syscall:fstat",
        "//nth/process/syscall:open",
        "//nth/process/syscall:lseek",
        "//nth/process/syscall:read",
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
    name = "reader",
    hdrs = ["reader.h"],
    deps = [],
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
        ":test",
        "//nth/test:main",
    ]
)

cc_library(
    name = "test",
    testonly = True,
    hdrs = ["test.h"],
    deps = [
        ":reader",
        "//nth/test",
    ],
)
