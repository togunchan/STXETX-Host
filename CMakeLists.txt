# CMakeLists.txt (root level)
cmake_minimum_required(VERSION 3.14)
project(SerialCommHost LANGUAGES CXX)

# Use the C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1) Add third-party libraries: spdlog and Catch2
add_subdirectory(third_party/spdlog)
add_subdirectory(third_party/catch2)

# 2) Include the "include" directory so that headers can be found
include_directories(${PROJECT_SOURCE_DIR}/include)

# 2.5) Enable CTest: This activates add_test(...) calls inside tests/CMakeLists.txt
enable_testing()

# 3) Collect all .cpp files inside src/ for compilation into a library
file(GLOB SRC_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# 4) Create a static library that compiles all modules together
add_library(SerialCommHostLib STATIC ${SRC_SOURCES})

# 5) If the Logger module uses spdlog, link spdlog to this library
target_link_libraries(SerialCommHostLib PRIVATE spdlog::spdlog)

# 6) Create the main application executable (only src/main.cpp)
add_executable(SerialCommHostApp
    src/main.cpp
)

# 7) Link the application to the SerialCommHostLib library
target_link_libraries(SerialCommHostApp PRIVATE SerialCommHostLib)

# 7.1) Stand-alone round-trip test
add_executable(test_Main src/test_Main.cpp)
target_link_libraries(test_Main PRIVATE SerialCommHostLib)

# 8) Add the tests/ subdirectory to include test configuration
add_subdirectory(tests)
