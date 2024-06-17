package(default_visibility = ["//visibility:public"])

cc_library(
    name = "address",
    hdrs = ["address.h"],
    deps = [
        "//nth/base:attributes",
    ],
)

cc_library(
    name = "buffer",
    hdrs = ["buffer.h"],
    deps = [
        "//nth/base:core",
    ],
)

cc_test(
    name = "buffer_test",
    srcs = ["buffer_test.cc"],
    deps = [
        ":buffer",
        "//nth/meta:type",
        "//nth/test:main",
    ],
)

cc_library(
    name = "bytes",
    hdrs = ["bytes.h"],
    deps = [
        ":address",
        "//nth/base:attributes",
    ],
)

cc_library(
    name = "offset_ptr",
    hdrs = ["offset_ptr.h"],
    deps = [],
)

cc_test(
    name = "offset_ptr_test",
    srcs = ["offset_ptr_test.cc"],
    deps = [
        ":offset_ptr",
        "//nth/test:main",
    ],
)


