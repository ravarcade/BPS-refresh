cmake_minimum_required(VERSION 3.5.0)

# VCPKG_TARGET_TRIPLET = x64-mingw-dynamic // debug
#                      = x64-mingw-static // release (?)
set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic")
project(ravtest VERSION 0.1.0)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror=reorder -Wno-unused-parameter -Wimplicit-fallthrough=0")

find_package(GTest CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spirv_cross_core CONFIG REQUIRED)
find_package(spirv_cross_glsl CONFIG REQUIRED)
find_package(spirv_cross_reflect CONFIG REQUIRED)
# find_package(spirv_cross_util CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)

set(LOG_LEVEL_TRACE 0)
set(LOG_LEVEL_DEBUG 1)
set(LOG_LEVEL_INFO 2)
add_compile_definitions(LOG_LEVEL LOG_LEVEL_TRACE)

add_subdirectory(libs)
add_subdirectory(executable)
