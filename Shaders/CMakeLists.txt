file(GLOB_RECURSE vs_file *.vert)
file(GLOB_RECURSE fs_file *.frag)
add_library(DQ_Shaders STATIC 
	"ShaderFactory.cpp"
	${vs_file}
	${fs_file}
)
target_include_directories(DQ_Shaders PUBLIC ${PROJECT_SOURCE_DIR}/Include)

set_target_properties(DQ_Shaders PROPERTIES FOLDER "DQ")