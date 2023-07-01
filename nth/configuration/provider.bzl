# All fields named in the `NthConfiguration` provider defined below.
NTH_CONFIGURATION_PROVIDER_FIELDS = ["verbosity"]

# `NthConfiguration` is a provider aggregating all global configuration options
# for the `nth` library.
NthConfiguration = provider(
    fields = NTH_CONFIGURATION_PROVIDER_FIELDS,
)
