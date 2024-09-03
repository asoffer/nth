package(default_visibility = ["//visibility:public"])

cc_library(
    name = "property",
    hdrs = ["property.h"],
    deps = [
        "//nth/debug/contracts",
        "//nth/debug/property/internal:concepts",
        "//nth/debug/property/internal:implementation",
        "//nth/debug/trace",
        "//nth/meta/concepts:invocable",
    ],
)

cc_test(
    name = "property_test",
    srcs = ["property_test.cc"],
    deps = [
        ":property",
        "//nth/debug/contracts",
    ],
)
