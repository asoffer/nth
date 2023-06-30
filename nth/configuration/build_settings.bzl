_CONFIG_HEADER_TEMPLATE = """
#ifndef NTH_CONFIGURATION_CONFIGURATION_GEN_H
#define NTH_CONFIGURATION_CONFIGURATION_GEN_H
{}
#endif  // NTH_CONFIGURATION_CONFIGURATION_GEN_H
"""
def _nth_configuration_header_impl(ctx):
    includes = [
        '#include "{}"'.format(h.path) for h in
        ctx.attr._configuration[CcInfo].compilation_context.direct_public_headers
    ]
    config_header = ctx.actions.declare_file(ctx.label.name + ".gen.h")
    ctx.actions.write(
        output = config_header,
        content = _CONFIG_HEADER_TEMPLATE.format('\n'.join(includes))
    )

    return [
        CcInfo(), 
        DefaultInfo(
        files = depset(
            [config_header],
            transitive = [
                ctx.attr._configuration[CcInfo].compilation_context.headers
            ])
    )]


nth_configuration_header = rule(
    implementation = _nth_configuration_header_impl,
    attrs = {
        "_configuration": attr.label(default = "//nth/configuration:target"),
    }
)
