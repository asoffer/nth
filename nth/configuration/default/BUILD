load("//nth/configuration:build_settings.bzl", "nth_configuration")

package(default_visibility = ["//nth/configuration:__subpackages__"])

cc_library(
    name = "log",
    hdrs = ["log.h"],
    deps = []
)

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
    deps = [
    ]
)

cc_library(
    name = "verbosity",
    hdrs = ["verbosity.h"],
    deps = [
        "//nth/debug/verbosity",
        "//nth/io:environment_variable",
    ]
)

nth_configuration(
    name = "default",
    log = ":log",
    verbosity = ":verbosity",
    trace = ":trace",
)
