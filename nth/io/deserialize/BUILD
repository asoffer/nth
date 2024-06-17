package(default_visibility = ["//visibility:public"])

cc_library(
    name = "deserialize",
    hdrs = ["deserialize.h"],
    deps = [
        "//nth/container:free_functions",
        "//nth/io/deserialize/internal:deserialize",
        "//nth/io/internal:sequence",
        "//nth/io/reader",
        "//nth/meta:concepts",
        "//nth/meta:type",
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
