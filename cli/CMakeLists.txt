
include_directories (
	"${CMAKE_SOURCE_DIR}/engine/text/internal/include"
	"${CMAKE_SOURCE_DIR}/engine/manager/include"
	)

add_executable (cli src/htstts.cpp)
target_link_libraries (cli manager)
