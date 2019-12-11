package(default_visibility = ["//visibility:public"])

cc_proto_library(
    name = "hello_proto_cc",
    deps = [":hello_proto"],
)

java_proto_library(
    name = "hello_proto_java",
    deps = [":hello_proto"],
)

proto_library(
    name = "hello_proto",
    srcs = ["autox/hello.proto"],
)
