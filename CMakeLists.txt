cmake_minimum_required( VERSION 3.0 )

project( twitch-cli )

include_directories( ./include ./source ./libs )

file( GLOB SOURCES ./source/*.c )

add_executable ( binary ${SOURCES} )

set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
