cmake_minimum_required(VERSION 3.15)
project(DQ)

set(CMAKE_CXX_STANDARD 20)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_subdirectory("Third/D3D12MemoryAllocator")
add_subdirectory("Window")
add_subdirectory("Graphics")
add_subdirectory("Examples/Window")