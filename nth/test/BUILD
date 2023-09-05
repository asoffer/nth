package(default_visibility = ["//visibility:public"])

cc_library(
    name = "arguments",
    hdrs = ["arguments.h"],
    visibility = ["//visibility:private"],
    testonly = True,
    deps = [
      "//nth/test/internal:arguments",
      "//nth/test/internal:invocation",
    ],
)

cc_test(
    name = "arguments_test",
    srcs = ["arguments_test.cc"],
    deps = [
        ":arguments",
    ],
)

cc_library(
    name = "fuzz",
    testonly = True,
    hdrs = ["fuzz.h"],
    deps = [
        ":generator",
        "@com_google_absl//absl/random",
    ],
)


cc_library(
    name = "generator",
    testonly = True,
    hdrs = ["generator.h"],
    deps = [
        ":arguments",
    ],
)

cc_library(
    name = "test",
    hdrs = ["test.h"],
    testonly = True,
    deps = [
      ":arguments",
      ":fuzz",
      "//nth/base:macros",
      "//nth/debug/property",
      "//nth/debug/trace",
      "//nth/debug/trace:testing",
      "//nth/test/internal:test",
    ],
)

cc_library(
    name = "main",
    srcs = ["main.cc"],
    testonly = True,
    deps = [
      ":test",
      "//nth/debug/log",
      "//nth/debug/log:stderr_log_sink",
      "//nth/utility:no_destructor",
      "@com_google_absl//absl/synchronization",
    ],
)
