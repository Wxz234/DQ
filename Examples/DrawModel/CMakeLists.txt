add_executable(DrawModel WIN32 DrawModel.cpp)
target_link_libraries(DrawModel PRIVATE DQ-Window DQ-Graphics DQ-Scene DQ-Renderer)
set_target_properties(DrawModel PROPERTIES FOLDER "Examples")