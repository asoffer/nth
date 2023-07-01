_CONFIG_HEADER_TEMPLATE = """
#ifndef NTH_CONFIGURATION_INTERNAL_{name}_GEN_H
#define NTH_CONFIGURATION_INTERNAL_{name}_GEN_H
{includes}
#endif  // NTH_CONFIGURATION_INTERNAL_{name}_GEN_H
"""


NthConfiguration = provider(
    fields = ["verbosity"],
)


def _nth_configuration_target_impl(ctx):
    target = getattr(ctx.attr._configuration[NthConfiguration], ctx.label.name)
    includes = [
        '#include "{}"'.format(h.path) for h in
        target[CcInfo].compilation_context.direct_public_headers
    ]
    config_header = ctx.actions.declare_file(ctx.label.name + ".h")
    ctx.actions.write(
        output = config_header,
        content = _CONFIG_HEADER_TEMPLATE.format(
            name = ctx.attr.name,
            includes = '\n'.join(includes)
        )
    )

    cc_info = CcInfo(
        compilation_context = cc_common.create_compilation_context(
            headers = depset(
                [config_header],
                transitive = [target[CcInfo].compilation_context.headers],
            )
        ),
        linking_context = target[CcInfo].linking_context,
    )
    return [
        cc_info, 
        DefaultInfo(
        files = depset(
            [config_header],
            transitive = [
                target[CcInfo].compilation_context.headers
            ])
    )]


nth_configuration_target = rule(
    implementation = _nth_configuration_target_impl,
    attrs = {
        "_configuration": attr.label(default = "//nth/configuration"),
    }
)


def _nth_configuration_impl(ctx):
    return [NthConfiguration(
        verbosity = ctx.attr.verbosity,
    )]
    

nth_configuration = rule(
    implementation = _nth_configuration_impl,
    attrs = {
        "verbosity": attr.label(default = "//nth/configuration:unconfigured_target"),
    }
)
