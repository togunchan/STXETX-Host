# CMakeLists.txt (root level)
cmake_minimum_required(VERSION 3.14)
project(SerialCommHost LANGUAGES CXX)

# Use C++17 standard
# (This ensures all targets are compiled with -std=c++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1) Add third-party libraries: spdlog and catch2
#    (These directories will be prepared later via git submodule or manual download)
add_subdirectory(third_party/spdlog)
add_subdirectory(third_party/catch2)

# 2) Include the "include" directory to access header files
#    (Adds the include directory from the project root to the include path)
include_directories(${PROJECT_SOURCE_DIR}/include)

# 3) Collect all .cpp files inside src/ for compilation
#    (file(GLOB ...) gathers files matching the specified pattern)
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# 4) Create the executable target
#    - The name of our serial communication host application: SerialCommHostApp
add_executable(SerialCommHostApp ${SOURCES})

# 5) Link the spdlog library
#    (Since the logger module depends on spdlog, we link it to the executable)
target_link_libraries(SerialCommHostApp PRIVATE spdlog::spdlog)

# 6) Add the tests/ directory to include unit tests in later stages
#    (Configuration will be handled in tests/CMakeLists.txt)
add_subdirectory(tests)
