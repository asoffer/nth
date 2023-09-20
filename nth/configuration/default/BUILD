load("//nth/configuration:build_settings.bzl", "nth_configuration")

package(default_visibility = ["//nth/configuration:__subpackages__"])


cc_library(
    name = "verbosity",
    hdrs = ["verbosity.h"],
    deps = [
        "//nth/debug/internal:verbosity",
        "//nth/io:environment_variable",
        "//nth/strings/format:universal",
    ]
)

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
    deps = [
        "//nth/strings/format:universal",
    ]
)

nth_configuration(
    name = "default",
    verbosity = ":verbosity",
    trace = ":trace",
)
