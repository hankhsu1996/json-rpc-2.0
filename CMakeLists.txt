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

# Fetch httplib
FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.16.0
)
FetchContent_MakeAvailable(httplib)

# Fetch Catch2
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.6.0
)
FetchContent_MakeAvailable(catch2)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the library
add_library(json_rpc_2_0 ${SOURCES})

# Link nlohmann_json
target_link_libraries(json_rpc_2_0 PUBLIC nlohmann_json::nlohmann_json spdlog::spdlog httplib::httplib)

# Unit tests
enable_testing()
add_subdirectory(tests)

# Stdio Examples
add_executable(stdio_client examples/stdio_client.cpp)
target_link_libraries(stdio_client PRIVATE json_rpc_2_0 nlohmann_json::nlohmann_json)

add_executable(stdio_server examples/stdio_server.cpp)
target_link_libraries(stdio_server PRIVATE json_rpc_2_0 nlohmann_json::nlohmann_json)

add_executable(http_client examples/http_client.cpp)
target_link_libraries(http_client PRIVATE json_rpc_2_0 nlohmann_json::nlohmann_json)

add_executable(http_server examples/http_server.cpp)
target_link_libraries(http_server PRIVATE json_rpc_2_0 nlohmann_json::nlohmann_json)
