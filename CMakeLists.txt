cmake_minimum_required(VERSION 3.15)
project(DQ)

set(CMAKE_CXX_STANDARD 20)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(Shaders)
add_subdirectory(Src)