add_executable(Scene WIN32 Scene.cpp)
target_link_libraries(Scene PRIVATE DQ-Window DQ-Graphics)
set_target_properties(Scene PROPERTIES FOLDER "Examples")