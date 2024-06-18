package(default_visibility = ["//visibility:public"])

cc_library(
    name = "format",
    hdrs = ["format.h"],
    deps = [
      ":interpolation_spec",
      "//nth/memory:bytes",
      "//nth/meta:type",
    ],
)

cc_library(
    name = "interpolation_spec",
    hdrs = ["interpolation_spec.h"],
    deps = [
        "//nth/strings/interpolate:string",
    ],
)
