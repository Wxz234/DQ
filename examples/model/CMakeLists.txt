add_executable(model WIN32 "model.cpp")
target_link_libraries(model PRIVATE DQ)
set_target_properties(model PROPERTIES FOLDER "Examples/Model")
set_target_properties(model PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_custom_command(TARGET model 
   POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E copy_directory  
       "${CMAKE_CURRENT_SOURCE_DIR}/Model"  
       "$<TARGET_FILE_DIR:model>"
) 