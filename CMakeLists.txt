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

# Add example executables
add_executable(stdio_client examples/stdio_client.cpp)
target_link_libraries(stdio_client PRIVATE jsonrpc)

add_executable(stdio_server examples/stdio_server.cpp)
target_link_libraries(stdio_server PRIVATE jsonrpc)

add_executable(pipe_client examples/pipe_client.cpp)
target_link_libraries(pipe_client PRIVATE jsonrpc)

add_executable(pipe_server examples/pipe_server.cpp)
target_link_libraries(pipe_server PRIVATE jsonrpc)

add_executable(framed_pipe_client examples/framed_pipe_client.cpp)
target_link_libraries(framed_pipe_client PRIVATE jsonrpc)

add_executable(framed_pipe_server examples/framed_pipe_server.cpp)
target_link_libraries(framed_pipe_server PRIVATE jsonrpc)

add_executable(socket_client examples/socket_client.cpp)
target_link_libraries(socket_client PRIVATE jsonrpc)

add_executable(socket_server examples/socket_server.cpp)
target_link_libraries(socket_server PRIVATE jsonrpc)

enable_testing()
add_subdirectory(tests)
