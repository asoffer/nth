package(default_visibility = ["//visibility:public"])

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
