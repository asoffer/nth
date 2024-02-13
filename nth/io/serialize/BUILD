package(default_visibility = ["//visibility:public"])

cc_library(
    name = "deserialize",
    hdrs = ["deserialize.h"],
    deps = [
        ":sequence",
        "//nth/container:free_functions",
        "//nth/meta:concepts",
        "//nth/io/reader",
        "//nth/utility:bytes",
    ],
)

cc_test(
    name = "round_trip_test",
    srcs = ["round_trip_test.cc"],
    deps = [
        ":deserialize",
        ":serialize",
        "//nth/container:flyweight_set",
        "//nth/io/writer:string",
        "//nth/io/reader:string",
        "//nth/test:main",
    ],
)

cc_library(
    name = "serialize",
    hdrs = ["serialize.h"],
    deps = [
        ":sequence",
        "//nth/io/writer",
        "//nth/container:free_functions",
        "//nth/utility:bytes",
    ],
)

cc_library(
    name = "sequence",
    hdrs = ["sequence.h"],
    visibility = ["//visibility:private"],
    deps = [],
)
