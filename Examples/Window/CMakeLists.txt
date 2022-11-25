add_executable(Window WIN32 Window.cpp)
target_link_libraries(Window PRIVATE DQ-Window DQ-Graphics)
set_target_properties(Window PROPERTIES FOLDER "Examples")