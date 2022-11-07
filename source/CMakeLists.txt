add_library(DQ STATIC 
	"DQ.cpp"
	"Device.cpp"
	"Window.cpp"
	"Scene.cpp"
	"Utility.cpp"
	"Renderer.cpp"
)

target_include_directories(DQ PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(DQ PRIVATE "${PROJECT_SOURCE_DIR}/thirdparty/cgltf" "${PROJECT_SOURCE_DIR}/thirdparty/stb" )
set_target_properties(DQ PROPERTIES FOLDER "DQ")
target_link_libraries(DQ PUBLIC dxgi.lib d3d12.lib dxguid.lib shlwapi.lib nvrhi nvrhi_d3d12)
target_compile_definitions(DQ PRIVATE _UNICODE UNICODE WIN32_LEAN_AND_MEAN)