package(default_visibility = ["//visibility:public"])

load("@rules_foreign_cc//tools/build_defs:cmake.bzl", "cmake_external")

cmake_external(
    name = "websocketpp",
    # Values to be passed as -Dkey=value on the CMake command line;
    # here are serving to provide some CMake script configuration options
    # cache_entries = {
    #     "NOFORTRAN": "on",
    #     "BUILD_WITHOUT_LAPACK": "no",
    # },
    headers_only = True,
    lib_source = "@websocketpp//:all",
    # deps = [":websocketpp"],
    # We are selecting the resulting static library to be passed in C/C++ provider
    # as the result of the build;
    # However, the cmake_external dependants could use other artefacts provided by the build,
    # according to their CMake script
    # static_libraries = ["libopenblas.a"],
)
