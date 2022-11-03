cmake_minimum_required(VERSION 3.15)
project(DQ)

if(MSVC)

	set_property( GLOBAL PROPERTY USE_FOLDERS ON)

	set(CMAKE_CXX_STANDARD 20)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS ON)

	set(NVRHI_WITH_VULKAN OFF CACHE BOOL "" FORCE)
	set(NVRHI_WITH_DX11 OFF CACHE BOOL "" FORCE)
	set(NVRHI_WITH_SHADER_COMPILER OFF CACHE BOOL "" FORCE)

	add_subdirectory("thirdparty/nvrhi")
	add_subdirectory("source")
	add_subdirectory("examples/hello_window")
	add_subdirectory("examples/model")
else()
	message(FATAL_ERROR "Please choose MSVC.")
endif()