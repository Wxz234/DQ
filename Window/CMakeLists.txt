add_library(DQ-Window STATIC "Window.cpp")
target_include_directories(DQ-Window PUBLIC "${PROJECT_SOURCE_DIR}/Include")
target_link_libraries(DQ-Window PUBLIC shlwapi.lib)
set_target_properties(DQ-Window PROPERTIES FOLDER "DQ")