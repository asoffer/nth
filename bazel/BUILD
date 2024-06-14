package(default_visibility = ["//visibility:public"])
load(":cc_toolchain_config.bzl", "cc_toolchain_config")

cc_toolchain_suite(
    name = "cc",
    toolchains = {
        "darwin": ":macosx_toolchain_x64",
        "darwin_arm64": ":macosx_toolchain_arm64",
        "k8": ":k8_toolchain",
    },
)

filegroup(name = "empty")

cc_toolchain(
    name = "macosx_toolchain_x64",
    toolchain_identifier = "clang-toolchain",
    toolchain_config = ":macosx_toolchain_x64_config",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
)

cc_toolchain(
    name = "macosx_toolchain_arm64",
    toolchain_identifier = "clang-toolchain",
    toolchain_config = ":macosx_toolchain_arm64_config",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
)

cc_toolchain_config(
    name = "macosx_toolchain_x64_config",
    os = "macosx",
    compiler_path = "/usr/local/opt/llvm/bin/clang",
    warnings = [
        "-Wall",
        "-Werror",
        "-Wextra",
        "-Wpedantic",
        "-Wuninitialized",
        "-Wno-c++2b-extensions",
        "-Wno-gcc-compat",
        "-Wno-gnu-statement-expression-from-macro-expansion",
    ]
)

cc_toolchain_config(
    name = "macosx_toolchain_arm64_config",
    os = "macosx",
    compiler_path = "/usr/local/bin/clang-16",
    warnings = [
        "-Wall",
        "-Werror",
        "-Wextra",
        "-Wpedantic",
        "-Wuninitialized",
        "-Wno-c++2b-extensions",
        "-Wno-gcc-compat",
        "-Wno-gnu-statement-expression-from-macro-expansion",
    ]
)

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
    compiler_path = "/usr/bin/clang++-16",
    warnings = [
        "-Wall",
        "-Werror",
        "-Wextra",
        "-Wpedantic",
        "-Wuninitialized",
        "-Wno-c++2b-extensions",
        "-Wno-gcc-compat",
        "-Wno-gnu-statement-expression-from-macro-expansion",
    ]
)
