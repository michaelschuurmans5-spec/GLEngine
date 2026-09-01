# Root CMakeLists
cmake_minimum_required(VERSION 3.15)
project(GLEngine VERSION 0.1.0 LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


add_subdirectory(external)


add_subdirectory(src)


