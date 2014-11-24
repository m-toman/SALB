
include_directories (
	"${CMAKE_SOURCE_DIR}/engine/text/internal/include"
	"${CMAKE_SOURCE_DIR}/engine/manager/include"
	)

add_library (textinternal
	src/AlbanianNormalizer.cpp
        src/AustrianGermanNormalizer.cpp
        src/Lexicon.cpp
        src/LTSTree.cpp
        src/Normalizer.cpp
	)
