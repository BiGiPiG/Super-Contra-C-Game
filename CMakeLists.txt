# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(SuperContra VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s -static -Os -Wall")

# Include directories
include_directories(
    "C:/raylib/raylib/src"
    "external"
)

# Find raylib package
find_library(RAYLIB_LIB raylib HINTS "C:/raylib/raylib/src")

# Add the executable
add_executable(${PROJECT_NAME} game.cpp)  # Change 'main.cpp' to your source file name

# Link libraries
target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIB} opengl32 gdi32 winmm)

# Optional: Set output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)