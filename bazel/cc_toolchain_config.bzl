load("@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
     "feature",
     "flag_group",
     "flag_set",
     "tool_path",
     "with_feature_set",
)
load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")

def make_flags(name, actions, flags):
    return feature(
        name = name,
        enabled = True,
        flag_sets = [
            flag_set(
                actions = actions,
                flag_groups = [flag_group(flags = flags)],
            ),
        ],
    )

ALL_COMPILE_ACTIONS = [
    ACTION_NAMES.assemble,
    ACTION_NAMES.preprocess_assemble,
    ACTION_NAMES.linkstamp_compile,
    ACTION_NAMES.c_compile,
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.cpp_header_parsing,
    ACTION_NAMES.cpp_module_codegen,
    ACTION_NAMES.cpp_module_compile,
    ACTION_NAMES.clif_match,
    ACTION_NAMES.lto_backend,
]

ALL_LINK_ACTIONS= [
    ACTION_NAMES.cpp_link_dynamic_library,
    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
    ACTION_NAMES.cpp_link_executable,
]


def std_lib_version(version):
    return make_flags(
        name = "std_lib_version",
        actions = [
            ACTION_NAMES.cpp_compile,
            ACTION_NAMES.cpp_header_parsing,
            ACTION_NAMES.cpp_module_codegen,
            ACTION_NAMES.cpp_module_compile,
        ],
        flags = ["-std=c++" + version],
    )


def mode_dependent_flags(dictionary):
    return feature(
        name = "mode_dependent_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [ACTION_NAMES.c_compile, ACTION_NAMES.cpp_compile],
                flag_groups = [flag_group(flags = value)],
                with_features = [with_feature_set(features = [key])],
            ) for (key, value) in dictionary.items()
        ],
    )


def compiler_flags(flags):
    return make_flags(
        name = "compiler_flags",
        actions = ALL_COMPILE_ACTIONS,
        flags = flags,
    )


def linking_flags(ls):
    return make_flags(
        name = "linking_flags",
        actions = ALL_LINK_ACTIONS,
        flags = ls,
    )

def _impl(ctx):
    tool_paths = [
        tool_path(name = "gcc",     path = ctx.attr.compiler_path),
        tool_path(name = "ld",      path = "/usr/bin/ld"),
        tool_path(name = "ar",      path = {
            "macosx": "/usr/bin/libtool",
        }.get(ctx.attr.os, "/usr/bin/ar")),
        tool_path(name = "cpp",     path = "/bin/false"),
        tool_path(name = "gcov",    path = "/bin/false"),
        tool_path(name = "nm",      path = "/bin/false"),
        tool_path(name = "objdump", path = "/bin/false"),
        tool_path(name = "strip",   path = "/bin/false"),
    ]
    features = [
        std_lib_version("2a"),
        compiler_flags(ctx.attr.warnings + [
            "-fbracket-depth=1024",
            "-fdiagnostics-color=always",
            "-fno-exceptions",
        ]),
        linking_flags([
            "-ldl",
            "-lm",
            "-lstdc++",
        ]),
        mode_dependent_flags({
            "dbg": ["-g", "-O0"],
            "opt": ["-O2"],
        }),

    ]

    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        toolchain_identifier = "cc-toolchain",
        host_system_name = "host",
        target_system_name = "target",
        target_cpu = "gcc",
        target_libc = {
          "macosx": "macosx",
        }.get(ctx.attr.os, "unknown"),
        compiler = "gcc",
        abi_version = "unknown",
        abi_libc_version = "unknown",
        cxx_builtin_include_directories = {
            "macosx": [
                "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/",
                "/Library/Developer/CommandLineTools/usr/lib/clang/13.1.6/include/"
            ],
        }.get(ctx.attr.os, [
            "/usr/lib",
            "/usr/include",
            "/usr/local/include",
        ]),
        tool_paths = tool_paths,
        features = features + [
            feature(name = "dbg"),
            feature(name = "fastbuild"),
            feature(name = "opt"),
        ],
    )

cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {
        "compiler_path": attr.string(),
        "os": attr.string(),
        "warnings": attr.string_list(),
    },
    provides = [CcToolchainConfigInfo],
)
