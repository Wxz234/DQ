add_library(DQ-Scene STATIC "Scene.cpp")
target_include_directories(DQ-Scene PUBLIC "${PROJECT_SOURCE_DIR}/Include")
set_target_properties(DQ-Scene PROPERTIES FOLDER "DQ")