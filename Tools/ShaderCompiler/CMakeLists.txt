add_executable(ShaderCompiler "ShaderCompiler.cpp")

target_compile_definitions(ShaderCompiler PRIVATE PROJECT_PATH="${PROJECT_SOURCE_DIR}")

set_target_properties(ShaderCompiler PROPERTIES FOLDER "Tools")