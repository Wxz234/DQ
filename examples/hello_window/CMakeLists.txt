add_executable(hello_window WIN32 "hello_window.cpp")
target_link_libraries(hello_window PRIVATE DQ)
set_target_properties(hello_window PROPERTIES FOLDER "Examples/Hello Window")