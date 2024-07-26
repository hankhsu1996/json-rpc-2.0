cmake_minimum_required(VERSION 3.19)
project(jsonrpc VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Find and link dependencies
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(httplib REQUIRED)
find_package(bshoshany-thread-pool REQUIRED)

# Add the library target
add_library(jsonrpc ${SOURCES})

# Include directories for the library
target_include_directories(jsonrpc PUBLIC include)

# Link dependencies
target_link_libraries(jsonrpc PUBLIC
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    httplib::httplib
    bshoshany-thread-pool::bshoshany-thread-pool
)

# Add example executables
add_executable(stdio_client examples/stdio_client.cpp)
target_link_libraries(stdio_client PRIVATE jsonrpc)

add_executable(stdio_server examples/stdio_server.cpp)
target_link_libraries(stdio_server PRIVATE jsonrpc)

add_executable(http_client examples/http_client.cpp)
target_link_libraries(http_client PRIVATE jsonrpc)

add_executable(http_server examples/http_server.cpp)
target_link_libraries(http_server PRIVATE jsonrpc)

enable_testing()
add_subdirectory(tests)
