cmake_minimum_required(VERSION 3.19)
project(jsonrpc VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set C++ Compiler Launcher
set(CMAKE_C_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Find and link dependencies
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(bshoshany-thread-pool REQUIRED)
find_package(asio REQUIRED)

# Add the library target
add_library(jsonrpc ${SOURCES})

# Include directories for the library
target_include_directories(jsonrpc PUBLIC include)

# Link dependencies
target_link_libraries(jsonrpc PUBLIC
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    bshoshany-thread-pool::bshoshany-thread-pool
    asio::asio
)

# Include examples CMake file
add_subdirectory(examples)

# Ensure compile_commands.json is moved after build
add_custom_command(
    TARGET jsonrpc PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMENT "Copying compile_commands.json to source directory"
    VERBATIM
)

enable_testing()
add_subdirectory(tests)
