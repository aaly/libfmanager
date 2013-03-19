# Search for gzip program
INCLUDE (FindGZIP.cmake)

# Compression of the manuals

message(STATUS "Compressing manuals with ${GZIP_TOOL}")

# Lists of source manuals
FILE(GLOB FMANAGER_MAN RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.3")
# Lists of compressed manuals
STRING(REGEX REPLACE ".3" ".3.gz" FMANAGER_MAN_GZ "${FMANAGER_MAN}")
STRING(REGEX REPLACE ".*/" "" FMANAGER_MAN_GZ "${FMANAGER_MAN_GZ}")

#message(FATAL_ERROR "${FMANAGER_MAN_GZ}")
# Compression of the manuals
FOREACH(man ${FMANAGER_MAN})
  ADD_CUSTOM_COMMAND(OUTPUT ${man}.gz
                     COMMAND ${GZIP_TOOL} -c "${CMAKE_CURRENT_SOURCE_DIR}/${man}" >  "${man}.gz"
                     DEPENDS ${man}
                     COMMENT "Building ${man}.gz")
ENDFOREACH(man)

# Add the manual generation in the global rules
ADD_CUSTOM_TARGET(man ALL
                  DEPENDS ${FMANAGER_MAN_GZ})

# Installation of the manuals
INSTALL(FILES ${FMANAGER_MAN_GZ}
        DESTINATION "usr/share/man/man3"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)







##add_custom_command(TARGET fmanager PRE_BUILD
##                   COMMAND ${CMAKE_COMMAND} -E copy_directory
#                       ${CMAKE_SOURCE_DIR}/doc $<TARGET_FILE_DIR:fmanager>/doc)
##                       ${CMAKE_SOURCE_DIR}/doc/man ${CMAKE_BINARY_DIR}/doc/man)

##add_custom_command(TARGET fmanager PRE_BUILD
                   #COMMAND for file in *; do gzip -c ${CMAKE_BINARY_DIR}/doc/man/$file > ${CMAKE_BINARY_DIR}/doc/man/"${file}.gz" && rm ${CMAKE_BINARY_DIR}/doc/man/$file; done )
##install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/man/ DESTINATION usr/share/man/man3)
