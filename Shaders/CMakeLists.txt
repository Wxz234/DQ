add_library(DQ_Shaders STATIC 
	"ShaderFactory.cpp"
	"gbuffer.vert"
)
target_include_directories(DQ_Shaders PUBLIC ${PROJECT_SOURCE_DIR}/Include)