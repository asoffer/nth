package(default_visibility = ["//visibility:public"])

cc_library(
    name = "try",
    hdrs = [
        "try.h",
        "internal/try.h",
    ],
    deps = [
        "//nth/base:attributes",
        "//nth/base:core",
        "//nth/base:macros",
        "//nth/debug/log",
        "//nth/meta:type",
        "//nth/meta/concepts:core",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
    ],
)

cc_test(
    name = "try_test",
    srcs = ["try_test.cc"],
    deps = [
        ":try",
        "//nth/test:main",
    ],
)
