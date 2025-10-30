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
        "//nth/types:structure",
        "//nth/test:main",
    ],
)

cc_library(
    name = "common",
    hdrs = ["common.h"],
    deps = [
      ":interpolate",
      ":format",
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
      "//nth/io/writer:string",
      "//nth/memory:bytes",
      "//nth/meta:constant",
      "//nth/meta:type",
      "//nth/types:structure",
    ],
)

cc_test(
    name = "format_test",
    srcs = ["format_test.cc"],
    deps = [
        ":format",
        "//nth/test:main",
        "@abseil-cpp//absl/strings",
        "@abseil-cpp//absl/strings:str_format",
    ],
)

cc_library(
    name = "interpolate",
    hdrs = ["interpolate.h"],
    srcs = ["internal/parameter_range.h"],
    deps = [
        ":format",
        "//nth/base:attributes",
        "//nth/base:core",
        "//nth/io/writer",
        "//nth/io/writer:string",
    ],
)

cc_test(
    name = "interpolate_test",
    srcs = ["interpolate_test.cc"],
    deps = [
        ":interpolate",
        "//nth/meta:type",
        "//nth/io/writer:string",
        "//nth/test/raw:test",
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
        ":structure",
        ":json",
        "//nth/test:main",
        "//nth/types:structure",
    ],
)

cc_library(
    name = "structure",
    hdrs = ["structure.h"],
    deps = [
        ":common",
        ":format",
        "//nth/types:structure",
        "//nth/meta/concepts:invocable",
    ],
)
