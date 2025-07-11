load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "attributes",
    hdrs = ["attributes.h"],
    deps = [
        ":macros",
    ],
)

cc_test(
    name = "attributes_test",
    srcs = ["attributes_test.cc"],
    deps = [":attributes"],
)

cc_library(
    name = "core",
    hdrs = ["core.h"],
    deps = [],
)

cc_test(
    name = "core_test",
    srcs = ["core_test.cc"],
    deps = [
        ":core",
    ],
)

cc_library(
    name = "indestructible",
    hdrs = ["indestructible.h"],
    deps = [
      ":core",
      "//nth/memory:address",
      "//nth/memory:buffer",
    ],
)

cc_test(
    name = "indestructible_test",
    srcs = ["indestructible_test.cc"],
    deps = [
        ":indestructible",
        "//nth/test/raw:test",
    ],
)

cc_library(
    name = "macros",
    hdrs = ["macros.h"],
    deps = [],
)

cc_test(
    name = "macros_test",
    srcs = ["macros_test.cc"],
    deps = [
        ":macros"
    ],
)

cc_library(
    name = "pack",
    hdrs = ["pack.h"],
    deps = [
        ":macros",
    ],
)

cc_test(
    name = "pack_test",
    srcs = ["pack_test.cc"],
    deps = [
        ":pack"
    ],
)

config_setting(
    name = "harden_build",
    visibility = ["//visibility:private"],
    flag_values = {"//nth/base:build": "harden"}
)

config_setting(
    name = "opt_build",
    visibility = ["//visibility:private"],
    flag_values = {"//nth/base:build": "optimize"}
)

config_setting(
    name = "dbg_build",
    visibility = ["//visibility:private"],
    flag_values = {"//nth/base:build": "debug"}
)

config_setting(
    name = "fast_build",
    visibility = ["//visibility:private"],
    flag_values = {"//nth/base:build": "fast"}
)


string_flag(
    name = "build",
    values = ["fast", "harden", "optimize", "debug"],
    build_setting_default = "fast",
)

cc_library(
    name = "platform",
    hdrs = ["platform.h"],
    defines = select({
        ":opt_build": ["NTH_CLI_BUILD_MODE_OPTIMIZE"],
        ":dbg_build": ["NTH_CLI_BUILD_MODE_DEBUG"],
        ":harden_build": ["NTH_CLI_BUILD_MODE_HARDEN"],
        "//conditions:default": ["NTH_CLI_BUILD_MODE_FASTBUILD"]
    }),
    deps = [
        ":macros",
    ],
)

cc_library(
    name = "section",
    hdrs = ["section.h"],
    deps = [
        ":macros",
        ":platform",
        "//nth/meta:compile_time_string",
    ],
)

cc_test(
    name = "section_test",
    srcs = [
        "section_test.cc",
        "internal/external_section.cc",
    ],
    deps = [
        ":section",
        "//nth/test/raw:test",
    ],
)
