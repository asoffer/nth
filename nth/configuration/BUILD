load("//nth/configuration:build_settings.bzl",
     "nth_configuration_header")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "unconfigured",
    hdrs = ["unconfigured.h"],
)


cc_library(
    name = "test",
    hdrs = ["test.h"],
)

label_flag(
    name = "target",
    build_setting_default = ":unconfigured",
)
 
nth_configuration_header(
    name = "header",
)

cc_library(
    name = "configuration",
    hdrs = ["configuration.h"],
    textual_hdrs = [":header"],
    visibility = ["//visibility:public"],
)
