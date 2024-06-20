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
    name = "configuration",
    hdrs = ["configuration.h"],
    deps = [
        ":macros",
    ],
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
    deps = [
        ":platform",
    ],
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

cc_library(
    name = "platform",
    hdrs = ["platform.h"],
    deps = [],
)

cc_library(
    name = "section",
    hdrs = ["section.h"],
    deps = [
        ":macros",
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
