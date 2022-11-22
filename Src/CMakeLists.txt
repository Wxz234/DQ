add_library(DQ_Core STATIC 
	"Scene.cpp"
	"APP.cpp"
	"Device.cpp"
	"WindowsBase.cpp"
	"Renderer.cpp"
)
target_include_directories(DQ_Core PUBLIC "${PROJECT_SOURCE_DIR}/Include")
target_include_directories(DQ_Core PRIVATE 
	"${PROJECT_SOURCE_DIR}/ThirdParty/Directx-Headers" 
	"${PROJECT_SOURCE_DIR}/ThirdParty/stb" 
	"${PROJECT_SOURCE_DIR}/ThirdParty/cgltf"
)
target_link_libraries(DQ_Core PUBLIC d3d12 dxgi dxguid shlwapi.lib)

set_target_properties(DQ_Core PROPERTIES FOLDER "DQ")