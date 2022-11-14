add_executable(Renderer WIN32 Renderer.cpp)

target_link_libraries(Renderer PRIVATE DQ_Core)

set_target_properties(Renderer PROPERTIES FOLDER "Examples")