package(default_visibility = ["//visibility:public"])

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
        "//nth/format",
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
