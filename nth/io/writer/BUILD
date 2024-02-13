package(default_visibility = ["//visibility:public"])

cc_library(
    name = "writer",
    hdrs = ["writer.h"],
    deps = [
        "//nth/utility:bytes",
    ],
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
