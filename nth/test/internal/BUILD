package(default_visibility = ["//nth/test:__subpackages__"])

cc_library(
    name = "arguments",
    hdrs = ["arguments.h"],
    testonly = True,
    deps = [],
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
    srcs = ["test.cc"],
    testonly = True,
    deps = [
      ":arguments",
      ":invocation",
      "//nth/meta:stateful",
      "//nth/strings:glob",
      "//nth/utility:registration",
    ],
)
