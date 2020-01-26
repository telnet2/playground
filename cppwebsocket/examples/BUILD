cc_binary(
    name = "main",
    srcs = ["main.cc"],
    copts = ["-std=c++11"],
    linkopts = ["-lpthread"],
    deps = [
        "//util:events",
        "//websocketpp",
        "@boost//:system",
    ],
)
