add_library(DQ_Core STATIC 
	"Scene.cpp"
	"APP.cpp"
	"Device.cpp"
	"WindowsBase.cpp"
	"Renderer.cpp"
)
target_include_directories(DQ_Core PUBLIC "${PROJECT_SOURCE_DIR}/Include")
target_include_directories(DQ_Core PRIVATE ${PROJECT_SOURCE_DIR}/ThirdParty/Directx-Headers)
target_link_libraries(DQ_Core PUBLIC d3d12 dxgi dxguid)

set_target_properties(DQ_Core PROPERTIES FOLDER "DQ")