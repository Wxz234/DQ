add_library(DQ-Renderer STATIC "Renderer.cpp")
target_include_directories(DQ-Renderer PUBLIC "${PROJECT_SOURCE_DIR}/Include")
target_link_libraries(DQ-Renderer PUBLIC DQ-Graphics)
set_target_properties(DQ-Renderer PROPERTIES FOLDER "DQ")