#ifndef NTH_CONFIGURATION_INTERNAL_UNCONFIGURED_TRACE_GENERATED_H
#define NTH_CONFIGURATION_INTERNAL_UNCONFIGURED_TRACE_GENERATED_H

#error \
Your build has a transitive dependency on the configurable library "trace" \
provided by `nth`, but it has not been configured. If you are compiling using \
Bazel, consider using \
`bazel cquery 'somepath(<your-target>, //nth/configuration/unconfigured:trace)'` \
to find the root cause of this dependency.

#endif  // NTH_CONFIGURATION_INTERNAL_UNCONFIGURED_TRACE_GENERATED_H
