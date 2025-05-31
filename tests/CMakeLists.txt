# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.14)

# Catch2 is already located under third_party/catch2
include_directories(${PROJECT_SOURCE_DIR}/include)

# Collect all .cpp test files inside the tests/ directory
# (file(GLOB ...) gathers files matching the specified pattern)
file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/tests/*.cpp")

# Create the test target (executable)
add_executable(SerialCommHostTests ${TEST_SOURCES})

# Link Catch2 and spdlog libraries
# (Catch2 is used for unit testing, and spdlog provides logging capabilities)
target_link_libraries(SerialCommHostTests PRIVATE Catch2::Catch2WithMain spdlog::spdlog)

# Enable CTest support to allow running tests with the `ctest` command
include(CTest)
add_test(NAME AllTests COMMAND SerialCommHostTests)
