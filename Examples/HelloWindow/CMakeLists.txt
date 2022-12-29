add_executable(HelloWindow WIN32 HelloWindow.cpp)
target_link_libraries(HelloWindow PRIVATE DQ)
set_target_properties(HelloWindow PROPERTIES FOLDER "Examples")