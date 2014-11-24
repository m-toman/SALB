
include_directories (
	"${CMAKE_SOURCE_DIR}/engine/manager/include"
	"${CMAKE_SOURCE_DIR}/engine/text/internal/include"
	"${CMAKE_SOURCE_DIR}/engine/text/flite/include"	
	"${CMAKE_SOURCE_DIR}/engine/synthesis/hts_engine/include"
	)

add_library (manager
   src/FliteTextAnalyzer.cpp
   src/HTSEngineSynthesizer.cpp
   src/InternalTextAnalyzer.cpp
   src/Label.cpp
   src/Synthesizer.cpp
   src/Text.cpp
   src/TextAnalyzer.cpp
   src/TTSLogger.cpp
   src/TTSManager.cpp
   src/utils.cpp
)

target_link_libraries (manager flite hts_engine textinternal)
