cmake_minimum_required(VERSION 3.10)
project(json_rpc_2_0 VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# FetchContent module
include(FetchContent)

# Fetch nlohmann_json
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(nlohmann_json)

# Fetch spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.14.1
)

FetchContent_MakeAvailable(spdlog)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the library
add_library(json_rpc_2_0 ${SOURCES})

# Link nlohmann_json
target_link_libraries(json_rpc_2_0 PUBLIC nlohmann_json::nlohmann_json spdlog::spdlog)

# Unit tests
enable_testing()
add_subdirectory(tests)

# Stdio Examples
add_executable(stdio_client examples/stdio_client.cpp)
target_link_libraries(stdio_client PRIVATE json_rpc_2_0 nlohmann_json::nlohmann_json)

add_executable(stdio_server examples/stdio_server.cpp)
target_link_libraries(stdio_server PRIVATE json_rpc_2_0 nlohmann_json::nlohmann_json)

add_executable(stdio_client_server examples/stdio_client_server.cpp)
target_link_libraries(stdio_client_server PRIVATE json_rpc_2_0 nlohmann_json::nlohmann_json)
find_package(Threads REQUIRED)
target_link_libraries(stdio_client_server PRIVATE Threads::Threads)
