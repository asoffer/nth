package(default_visibility = ["//nth/test:__subpackages__"])

cc_library(
    name = "arguments",
    hdrs = ["arguments.h"],
    testonly = True,
    deps = [],
)

cc_library(
    name = "benchmark",
    testonly = True,
    hdrs = ["benchmark.h"],
    srcs = ["benchmark.cc"],
    deps = [
        "//nth/test:benchmark_result",
        "//nth/meta:compile_time_string",
    ],
)

cc_library(
    name = "invocation",
    hdrs = ["invocation.h"],
    testonly = True,
    deps = [],
)

cc_library(
    name = "test",
    hdrs = ["test.h"],
    testonly = True,
    deps = [
      ":arguments",
      ":invocation",
      "//nth/base:macros",
      "//nth/meta:stateful",
      "//nth/strings:glob",
      "//nth/registration:static",
    ],
)
