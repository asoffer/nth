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
    name = "test",
    hdrs = ["test.h"],
    testonly = True,
    deps = [
      ":arguments",
      "//nth/base:macros",
      "//nth/debug/trace",
      "//nth/debug/trace:matcher",
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
    ],
)
