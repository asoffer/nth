package(default_visibility = ["//visibility:public"])

cc_library(
    name = "deserialize",
    hdrs = ["deserialize.h"],
    deps = [
        "//nth/io/deserialize/internal:deserialize",
        "//nth/io/internal:sequence",
        "//nth/container:free_functions",
        "//nth/meta:concepts",
        "//nth/io/reader",
        "//nth/utility:bytes",
    ],
)

cc_test(
    name = "deserialize_test",
    srcs = ["deserialize_test.cc"],
    deps = [
        ":deserialize",
        "//nth/base:platform",
        "//nth/test:main",
    ],
)
