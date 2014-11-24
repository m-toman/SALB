include_directories (
	"${CMAKE_SOURCE_DIR}/engine/synthesis/hts_engine/include"
	"${CMAKE_SOURCE_DIR}/engine/synthesis/hts_engine/lib"	
       )

add_library (hts_engine 
	lib/HTS_audio.c
	lib/HTS_engine.c
	lib/HTS_gstream.c
	lib/HTS_label.c
	lib/HTS_misc.c
	lib/HTS_model.c
	lib/HTS_pstream.c
	lib/HTS_sstream.c
	lib/HTS_vocoder.c
	)

IF(WIN32)
    TARGET_LINK_LIBRARIES(hts_engine winmm)
ENDIF(WIN32)

