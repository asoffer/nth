load("@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
     "action_config",
     "feature",
     "flag_group",
     "flag_set",
     "tool",
     "tool_path",
     "with_feature_set",
)

load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")

def _toolchain_id(os, cpu):
    return "{}_{}-toolchain".format(os, cpu)

def _make_flags(name, actions, flags):
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


_ALL_C_COMPILE_ACTIONS = [
    ACTION_NAMES.c_compile,
    ACTION_NAMES.assemble,
    ACTION_NAMES.preprocess_assemble,
]


_ALL_CC_COMPILE_ACTIONS = [
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.linkstamp_compile,
    ACTION_NAMES.cpp_header_parsing,
    ACTION_NAMES.cpp_module_compile,
    ACTION_NAMES.cpp_module_codegen,
]

_ALL_CODEGEN_ACTIONS = [
    ACTION_NAMES.c_compile,
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.linkstamp_compile,
    ACTION_NAMES.assemble,
    ACTION_NAMES.preprocess_assemble,
    ACTION_NAMES.cpp_module_codegen,
]

_ALL_COMPILE_ACTIONS = _ALL_CC_COMPILE_ACTIONS + _ALL_C_COMPILE_ACTIONS

def _warnings(flags, system_headers):
    return _make_flags(
        name = "warnings",
        actions = _ALL_CC_COMPILE_ACTIONS + _ALL_CC_COMPILE_ACTIONS,
        flags = flags + ["--system-header-prefix=" + d for d in system_headers],
    )


def _std_lib_version(version):
    return _make_flags(
        name = "std_lib_version",
        actions = _ALL_CC_COMPILE_ACTIONS,
        flags = ["-std=c++" + version],
    )


def _common_flags():
    return feature(
        name = "common_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = _ALL_COMPILE_ACTIONS,
                flag_groups = ([
                    flag_group(
                        flags = [
                            "-fcolor-diagnostics",
                        ],
                    ),
                ]),
            ),
        ],
    )


def _opt_flags():
    return feature(
        name = "opt_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = _ALL_CODEGEN_ACTIONS,
                flag_groups = ([
                    flag_group(
                        flags = ["-O3"],
                    ),
                ]),
                with_features = [with_feature_set(features = ["opt"])],
            ),
        ]
    )


def _debug_flags():
    return feature(
        name = "debug_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = _ALL_CODEGEN_ACTIONS,
                flag_groups = ([
                    flag_group(
                        flags = ["-g"],
                    ),
                ]),
                with_features = [with_feature_set(features = ["dbg"])],
            ),
            flag_set(
                actions = _ALL_CODEGEN_ACTIONS,
                flag_groups = [
                    flag_group(
                        flags = ["-gsplit-dwarf", "-g"],
                        expand_if_available = "per_object_debug_info_file",
                    ),
                ],
            ),
        ]
    )


def _impl(ctx):
    features = [
        feature(name = "dbg"),
        feature(name = "opt"),
        _std_lib_version("20"),
        _common_flags(),
        _debug_flags(),
        _opt_flags(),
        _warnings(getattr(ctx.attr, "warnings", []),
                  getattr(ctx.attr, "system_headers", [])),

    ]

    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        toolchain_identifier = _toolchain_id(ctx.attr.os, ctx.attr.cpu),
        host_system_name = "local",
        target_system_name = "local",
        target_cpu = ctx.attr.cpu,
        target_libc = "unknown",
        compiler = "clang",
        abi_version = "unknown",
        abi_libc_version = "unknown",
        cxx_builtin_include_directories = ctx.attr.builtin_include_directories,
        tool_paths = [
            tool_path(name = "gcc", path = ctx.attr.cc_compiler_path),
            tool_path(name = "ld", path = "/usr/bin/ld"),
            tool_path(name = "ar", path = ctx.attr.ar_path),
            tool_path(name = "cpp", path = "/bin/false"),
            tool_path(name = "gcov", path = "/bin/false"),
            tool_path(name = "nm", path = "/bin/false"),
            tool_path(name = "objdump", path = "/bin/false"),
            tool_path(name = "strip", path = "/bin/false"),
        ],
        action_configs = [
            action_config(action_name = name, enabled = True, tools = [
                tool(path = ctx.attr.c_compiler_path)
            ])
            for name in _ALL_C_COMPILE_ACTIONS
        ] + [
            action_config(action_name = name, enabled = True, tools = [
                tool(path = ctx.attr.cc_compiler_path)
            ])
            for name in _ALL_CC_COMPILE_ACTIONS
        ],
        features = features,
    )


_cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {
        "ar_path": attr.string(mandatory = True),
        "c_compiler_path": attr.string(mandatory = True),
        "cc_compiler_path": attr.string(mandatory = True),
        "builtin_include_directories": attr.string_list(mandatory = True),
        "os": attr.string(mandatory = True),
        "cpu": attr.string(mandatory = True),
        "system_headers": attr.string_list(),
        "warnings": attr.string_list(),
    },
    provides = [CcToolchainConfigInfo],
)

def clang_toolchain(name, **kwargs):
    os = kwargs["os"]
    cpu = kwargs["cpu"]
    toolchain_name = "clang_toolchain_{}_{}_toolchain".format(os, cpu)
    toolchain_id = _toolchain_id(os, cpu)

    _cc_toolchain_config(
        name = toolchain_name + "_config",
        **kwargs
    )

    native.cc_toolchain(
        name = toolchain_name,
        toolchain_identifier = toolchain_id,
        toolchain_config = toolchain_name + "_config",
        all_files = ":empty",
        compiler_files = ":empty",
        dwp_files = ":empty",
        linker_files = ":empty",
        objcopy_files = ":empty",
        strip_files = ":empty",
        supports_param_files = 0,
    )

    native.toolchain(
        name = name,
        toolchain = toolchain_name,
        toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
        exec_compatible_with = [
            "@platforms//cpu:{}".format(cpu),
            "@platforms//os:{}".format(os),
        ],
        target_compatible_with = [
            "@platforms//cpu:{}".format(cpu),
            "@platforms//os:{}".format(os),
        ],
    )
