package(default_visibility = ["//visibility:public"])

cc_library(
    name = "round_trip",
    testonly = True,
    hdrs = ["round_trip.h"],
    deps = [
        "//nth/io/deserialize",
        "//nth/io/serialize",
        "//nth/io/writer:string",
        "//nth/io/reader:string",
        "//nth/test",
    ],
)

cc_test(
    name = "round_trip_test",
    srcs = ["round_trip_test.cc"],
    deps = [
        "//nth/container:flyweight_set",
        "//nth/io/deserialize",
        "//nth/io/serialize",
        "//nth/io/writer:string",
        "//nth/io/reader:string",
        "//nth/test:main",
    ],
)

cc_library(
    name = "serialize",
    hdrs = ["serialize.h"],
    deps = [
        "//nth/io/internal:sequence",
        "//nth/io/serialize/internal:serialize",
        "//nth/io/writer",
        "//nth/container:free_functions",
    ],
)

cc_test(
    name = "serialize_test",
    srcs = ["serialize_test.cc"],
    deps = [
        ":serialize",
        "//nth/base:platform",
        "//nth/test:main",
    ],
)
