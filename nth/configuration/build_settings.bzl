load("//nth/configuration:provider.bzl",
     "NTH_CONFIGURATION_PROVIDER_FIELDS",
     "NthConfiguration")


# Template for aggregating configuration header.
_CONFIG_HEADER_TEMPLATE = """
#ifndef NTH_CONFIGURATION_INTERNAL_{name}_GENERATED_H
#define NTH_CONFIGURATION_INTERNAL_{name}_GENERATED_H
{includes}
#endif  // NTH_CONFIGURATION_INTERNAL_{name}_GENERATED_H
"""

def _extract_external(path):
    if path.startswith("external/"):
        pos = path.find("/", len("external/")) + 1
        return path[pos:]
    else:
        return path


def _nth_configuration_target_impl(ctx):
    target = getattr(ctx.attr._configuration[NthConfiguration], ctx.label.name)
    config_header = ctx.actions.declare_file(ctx.label.name + ".h")
    ctx.actions.write(
        output = config_header,
        content = _CONFIG_HEADER_TEMPLATE.format(
            name = ctx.attr.name,
            includes = '\n'.join([
                '#include "{}"'.format(_extract_external(h.path)) for h in
                target[CcInfo].compilation_context.headers.to_list()
            ]),
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
                transitive = [cc_info.compilation_context.headers]
            )
        )
    ]


_nth_configuration_target = rule(
    implementation = _nth_configuration_target_impl,
    attrs = {
        "_configuration": attr.label(default = "//nth/configuration"),
    }
)


def _nth_configuration_impl(ctx):
    return [NthConfiguration(**{
        field: getattr(ctx.attr, field)
        for field in NTH_CONFIGURATION_PROVIDER_FIELDS
    })]
    

nth_configuration = rule(
    implementation = _nth_configuration_impl,
    attrs = {
        field: attr.label(
            default = "//nth/configuration/unconfigured:{}".format(field)
        )
        for field in NTH_CONFIGURATION_PROVIDER_FIELDS
    }
)


def nth_define_configuration_targets():
    for field in  NTH_CONFIGURATION_PROVIDER_FIELDS:
        _nth_configuration_target(
            name = field,
            visibility = ["//nth:__subpackages__"],
        )
