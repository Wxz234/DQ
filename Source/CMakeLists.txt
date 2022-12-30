add_library(DQ STATIC 
	"Window.cpp"
	"Device.cpp"
)

target_include_directories(DQ PUBLIC "${PROJECT_SOURCE_DIR}/Include")
target_link_libraries(DQ PUBLIC d3d12.lib dxgi.lib dxguid.lib)
set_target_properties(DQ PROPERTIES FOLDER "DQ")