add_executable(HelloWorld WIN32 HelloWorld.cpp)

target_link_libraries(HelloWorld PRIVATE DQ_Core)

set_target_properties(HelloWorld PROPERTIES FOLDER "Examples")