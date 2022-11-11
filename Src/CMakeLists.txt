add_library(DQ_Core STATIC 
	"Scene.cpp"
	"Scene.h"
)
target_include_directories(DQ_Core PUBLIC ${PROJECT_SOURCE_DIR}/Include)

set_target_properties(DQ_Core PROPERTIES FOLDER "DQ")