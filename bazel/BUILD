package(default_visibility = ["//visibility:public"])
load(":cc_toolchain_config.bzl", "cc_toolchain_config")

cc_toolchain_suite(
    name = "cc",
    toolchains = {
        "k8": ":k8_toolchain",
    },
)

filegroup(name = "empty")

cc_toolchain(
    name = "k8_toolchain",
    toolchain_identifier = "clang-toolchain",
    toolchain_config = ":k8_toolchain_config",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
)

cc_toolchain_config(
    name = "k8_toolchain_config",
    os = "k8",
    compiler_path = "/usr/bin/clang-14",
    warnings = [
        "-Wall",
        "-Werror",
        "-Wextra",
        "-Wpedantic",
        "-Wuninitialized",
        "-Wno-gcc-compat", # https://github.com/abseil/abseil-cpp/issues/1360
    ]
)
