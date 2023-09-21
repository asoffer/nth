package(default_visibility = ["//nth/debug:__subpackages__"])

cc_library(
    name = "concepts",
    hdrs = ["concepts.h"],
    visibility = ["//nth/debug:__subpackages__"],
    deps = []
)

cc_library(
    name = "implementation",
    hdrs = ["implementation.h"],
    deps = [
      ":concepts",
      "//nth/meta:compile_time_string",
    ],
)
