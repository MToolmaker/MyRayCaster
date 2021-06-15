cmake_minimum_required(VERSION 3.17)
project(untitled2)

set(CMAKE_CXX_STANDARD 17)
include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/tests/include")
add_executable(untitled2 main.cpp include/olcPixelGameEngine.h)
target_link_libraries(untitled2 -lOpenGL32 -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi)