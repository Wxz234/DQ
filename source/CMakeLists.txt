add_library(DQ STATIC 
	"DQ.cpp"
	"Scene.cpp"
	"Device.cpp"
	"Window.cpp"
	"Texture.cpp"
)

target_include_directories(DQ PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(DQ PRIVATE "${PROJECT_SOURCE_DIR}/thirdparty/cgltf")
set_target_properties(DQ PROPERTIES FOLDER "DQ")
target_link_libraries(DQ PUBLIC dxgi.lib d3d12.lib dxguid.lib nvrhi nvrhi_d3d12)