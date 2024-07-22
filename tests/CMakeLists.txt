# Add Catch2
find_package(Catch2 3.6.0 REQUIRED)

# Update CMAKE_MODULE_PATH to include Catch2 extras
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

# Add test files
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# Create a test executable
add_executable(tests ${TEST_SOURCES})

# Link with the main project and Catch2
target_link_libraries(tests PRIVATE json_rpc_2_0 Catch2::Catch2WithMain)

# Register the tests
catch_discover_tests(tests)
