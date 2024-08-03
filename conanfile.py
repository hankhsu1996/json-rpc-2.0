from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class JsonRpcConan(ConanFile):
    name = "json-rpc-2.0"
    version = "1.0.0"
    license = "MIT"
    author = "Shou-Li Hsu <hank850503@gmail.com>"
    url = "https://github.com/hankhsu1996/json-rpc-2.0"
    description = (
        "Welcome to the JSON-RPC 2.0 Modern C++ Library! "
        "This library provides a lightweight, modern C++ implementation of "
        "JSON-RPC 2.0 servers and clients. It is designed to be flexible, allowing "
        "integration with various transport layers. This library makes it easy to "
        "register methods and notifications, binding them to client logic efficiently."
    )
    topics = ("json-rpc", "rpc", "json", "modern-c++", "networking")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Declare package dependencies
    requires = [
        "nlohmann_json/3.11.2",
        "spdlog/1.9.2",
        "bshoshany-thread-pool/4.1.0"
    ]

    tool_requires = [
        "ninja/1.12.1",
        "ccache/4.10"
    ]

    test_requires = [
        "catch2/3.6.0"
    ]

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*"

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.user_presets_path = 'ConanPresets.json'

        # Set up ccache and Ninja
        tc.variables["CMAKE_C_COMPILER_LAUNCHER"] = "ccache"
        tc.variables["CMAKE_CXX_COMPILER_LAUNCHER"] = "ccache"
        tc.generator = "Ninja"

        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
