file(GLOB_RECURSE vs_file *.vert)
file(GLOB_RECURSE fs_file *.frag)

set_source_files_properties(gbuffer.vert PROPERTIES Entry "main")

add_library(DQ_Shaders STATIC 
	"ShaderFactory.cpp"
	${vs_file}
	${fs_file}
)
target_include_directories(DQ_Shaders PUBLIC ${PROJECT_SOURCE_DIR}/Include)

set_target_properties(DQ_Shaders PROPERTIES FOLDER "DQ")

foreach(myfile ${vs_file})
	get_filename_component(variable_name ${myfile} NAME_WE ABSOLUTE)
	get_source_file_property(MyEntry ${myfile} Entry)
	if(${MyEntry} STREQUAL NOTFOUND)
		add_custom_command(TARGET DQ_Shaders PRE_BUILD 
			COMMAND 
			"${PROJECT_SOURCE_DIR}/ThirdParty/DirectXShaderCompiler/bin/x64/dxc.exe" 
			${myfile}
			"/E"
			"main"
			"/T"
			"vs_6_6"
			"/Fh"
			"${PROJECT_SOURCE_DIR}/Include/DQ/Shaders/${variable_name}.vert.h"
			"/Vn"
			"${variable_name}_vs"
		)
	else()
		add_custom_command(TARGET DQ_Shaders PRE_BUILD 
			COMMAND 
			"${PROJECT_SOURCE_DIR}/ThirdParty/DirectXShaderCompiler/bin/x64/dxc.exe" 
			${myfile}
			"/E"
			${MyEntry}
			"/T"
			"vs_6_6"
			"/Fh"
			"${PROJECT_SOURCE_DIR}/Include/DQ/Shaders/${variable_name}.vert.h"
			"/Vn"
			"${variable_name}_vs"
		)
	endif()
endforeach()

foreach(myfile ${fs_file})
	get_filename_component(variable_name ${myfile} NAME_WE ABSOLUTE)
	get_source_file_property(MyEntry ${myfile} Entry)
	if(${MyEntry} STREQUAL NOTFOUND)
		add_custom_command(TARGET DQ_Shaders PRE_BUILD 
			COMMAND 
			"${PROJECT_SOURCE_DIR}/ThirdParty/DirectXShaderCompiler/bin/x64/dxc.exe" 
			${myfile}
			"/E"
			"main"
			"/T"
			"ps_6_6"
			"/Fh"
			"${PROJECT_SOURCE_DIR}/Include/DQ/Shaders/${variable_name}.frag.h"
			"/Vn"
			"${variable_name}_ps"
		)
	else()
		add_custom_command(TARGET DQ_Shaders PRE_BUILD 
			COMMAND 
			"${PROJECT_SOURCE_DIR}/ThirdParty/DirectXShaderCompiler/bin/x64/dxc.exe" 
			${myfile}
			"/E"
			${MyEntry}
			"/T"
			"ps_6_6"
			"/Fh"
			"${PROJECT_SOURCE_DIR}/Include/DQ/Shaders/${variable_name}.frag.h"
			"/Vn"
			"${variable_name}_ps"
		)
	endif()
endforeach()