cmake_minimum_required(VERSION 2.8)

PROJECT(libfmanager)

add_library(fmanager SHARED libfmanager.cpp)
install (TARGETS fmanager DESTINATION usr/lib)
add_library(fmanagerstatic STATIC libfmanager.cpp)
install (TARGETS fmanagerstatic DESTINATION usr/lib)
IF(NOT WIN32) # static or shared, windows uses the same extension not .a and .so :(
	set_target_properties(fmanagerstatic PROPERTIES OUTPUT_NAME fmanager)
ENDIF(NOT WIN32)

########## Docs ################
add_subdirectory(doc)
######### Header ##############
SET(FMANAGER_HEADER libfmanager.hpp)
INSTALL(FILES ${FMANAGER_HEADER}
        DESTINATION "usr/lib/"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
##add_custom_target(FMANAGER_HEADER SOURCES libfmanager.hpp)
##install (TARGETS FMANAGER_HEADER DESTINATION /usr/share/man)

#
#add_custom_command(TARGET fmanager POST_BUILD
#		   COMMAND ${CMAKE_COMMAND} -E copy
#		   ${CMAKE_SOURCE_DIR}/${FMANAGER_HEADER}  ${CMAKE_BINARY_DIR})
