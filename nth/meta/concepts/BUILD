package(default_visibility = ["//visibility:public"])

cc_library(
    name = "c_array",
    hdrs = ["c_array.h"],
    deps = [],
)

cc_test(
    name = "c_array_test",
    srcs = ["c_array_test.cc"],
    deps = [":c_array"],
)

cc_library(
    name = "comparable",
    hdrs = ["comparable.h"],
    deps = [":core"],
)

cc_test(
    name = "comparable_test",
    srcs = ["comparable_test.cc"],
    deps = [":comparable"],
)

cc_library(
    name = "convertible",
    hdrs = ["convertible.h"],
    deps = [],
)

cc_test(
    name = "convertible_test",
    srcs = ["convertible_test.cc"],
    deps = [":convertible"],
)

cc_library(
    name = "constructible",
    hdrs = ["constructible.h"],
    deps = [],
)

cc_test(
    name = "constructible_test",
    srcs = ["constructible_test.cc"],
    deps = [":constructible"],
)

cc_library(
    name = "core",
    hdrs = ["core.h"],
    deps = [],
)

cc_test(
    name = "core_test",
    srcs = ["core_test.cc"],
    deps = [":core"],
)

cc_library(
    name = "hash",
    hdrs = ["hash.h"],
    deps = [
        ":convertible",
        ":invocable",
    ],
)

cc_test(
    name = "hash_test",
    srcs = ["hash_test.cc"],
    deps = [":hash"],
)

cc_library(
    name = "invocable",
    hdrs = ["invocable.h"],
    deps = [":core"],
)

cc_test(
    name = "invocable_test",
    srcs = ["invocable_test.cc"],
    deps = [":invocable"],
)
