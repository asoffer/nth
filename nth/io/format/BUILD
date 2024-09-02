package(default_visibility = ["//visibility:public"])

cc_library(
    name = "cc",
    hdrs = ["cc.h"],
    deps = [
        ":common",
        ":format",
        "//nth/container:stack",
        "//nth/types:structure",
    ],
)

cc_test(
    name = "cc_test",
    srcs = ["cc_test.cc"],
    deps = [
        ":cc",
        "//nth/test:main",
    ],
)


cc_library(
    name = "common",
    hdrs = ["common.h"],
    deps = [
      ":format",
      "//nth/format/interpolate:string",
    ],
)

cc_library(
    name = "format",
    hdrs = ["format.h"],
    srcs = [
        "common_formatters.h",
        "common_defaults.h",
    ],
    deps = [
      "//nth/io/writer",
      "//nth/memory:bytes",
      "//nth/meta:constant",
      "//nth/meta:type",
      "//nth/types:structure",
    ],
)

cc_library(
    name = "json",
    hdrs = ["json.h"],
    deps = [
        ":common",
        ":format",
        "//nth/container:stack",
        "//nth/types:structure",
    ],
)

cc_test(
    name = "json_test",
    srcs = ["json_test.cc"],
    deps = [
        ":json",
        "//nth/test:main",
    ],
)

