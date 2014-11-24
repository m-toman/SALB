set (CMAKE_LEGACY_CYGWIN_WIN32 1)

cmake_minimum_required (VERSION 2.6)
project (SALB)

# compiler stuff
set (CMAKE_CXX_FLAGS "-std=c++0x")


# specify output subdirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release/bin)


# hts_engine
add_subdirectory (engine/synthesis/hts_engine) 

# flite
add_subdirectory (engine/text/flite) 

# internal text analyzer
add_subdirectory (engine/text/internal) 

# manager
add_subdirectory (engine/manager)

# CLI
add_subdirectory (cli)

