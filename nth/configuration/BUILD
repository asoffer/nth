load("//nth/configuration:build_settings.bzl", "nth_define_configuration_targets")

package(default_visibility = ["//visibility:private"])

# Command-line configuration setting. Users may specify
# `--//nth/configuration=<some-target>` either on the command as part of a Bazel
# invocation or in a `.bazelrc` file. The specified target must provide an 
# `NthConfiguration` provider.
#
# There are two built-in configurations provided:
#   * `//nth/configuration/default`: 
#         Provides a somewhat opinionated set of defaults we believe to be
#         reasonable for most use cases.
#   * `//nth/configuration/unconfigured`:
#         Provides no configuration whatsoever. Any target using any portion of
#         this configuration will fail to compile.
#
#  If no such target is configured, the target
# `//nth/configuration/default` will be assumed.
label_flag(
    name = "configuration",
    build_setting_default = "//nth/configuration/default",
)

# Defines configuration targets accessible to libraries that depend on aspects
# of the configuration. For each field in the `NthConfiguration` provider, a
# target of the corresponding name is defined, giving access to the
# corresponding configuration. For example, one could depend on 
# `//nth/configuration:verbosity` to have access to the configuration for debug 
# verbosities.
#
# NOTE: configurations are private to the `nth` libraries unless otherwise
# explicitly specified and may not be directly depended on by outside libraries.
nth_define_configuration_targets()
