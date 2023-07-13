package(default_visibility = ["//visibility:public"])

cc_library(
    name = "test",
    hdrs = ["test.h"],
    testonly = True,
    srcs = ["test.cc"],
    deps = [
      "//nth/base:macros",
      "//nth/debug/trace",
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
