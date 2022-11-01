add_executable(scene WIN32 "scene.cpp")
target_link_libraries(scene PRIVATE DQ)
set_target_properties(scene PROPERTIES FOLDER "Examples/Scene")
set_target_properties(scene PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_custom_command(TARGET scene 
   POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E copy_directory  
       "${CMAKE_CURRENT_SOURCE_DIR}/Model"  
       "$<TARGET_FILE_DIR:scene>"
) 