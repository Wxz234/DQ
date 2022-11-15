add_executable(ShaderCompiler "ShaderCompiler.cpp")

target_compile_definitions(ShaderCompiler PRIVATE PROJECT_PATH="${PROJECT_SOURCE_DIR}")