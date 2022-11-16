file(GLOB_RECURSE all_shader_file ${PROJECT_SOURCE_DIR}/Shaders/*)

foreach(shader_file ${all_shader_file})
	set_source_files_properties(${shader_file} PROPERTIES VS_TOOL_OVERRIDE "None")
endforeach()

add_executable(ShaderCompiler "ShaderCompiler.cpp" ${all_shader_file})

target_compile_definitions(ShaderCompiler PRIVATE PROJECT_PATH="${PROJECT_SOURCE_DIR}")

set_target_properties(ShaderCompiler PROPERTIES FOLDER "Tools")