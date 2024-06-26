###############################################################################
#                                                                             #
# Copyright (C) 2022 Project Nighthold <https://github.com/ProjectNighthold>  #
#                                                                             #
# This file is free software; as a special exception the author gives         #
# unlimited permission to copy and/or distribute it, with or without          #
# modifications, as long as this notice is preserved.                         #
#                                                                             #
# This program is distributed in the hope that it will be useful, but         #
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the      #
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    #
#                                                                             #
# Read the THANKS file on the source root directory for more info.            #
#                                                                             #
###############################################################################

include_directories(
  ${CMAKE_SOURCE_DIR}/src/common
  ${CMAKE_SOURCE_DIR}/src/common/collision
  ${CMAKE_SOURCE_DIR}/src/common/collision/Maps
  ${CMAKE_SOURCE_DIR}/dep/g3dlite/include
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Debugging
  ${CMAKE_SOURCE_DIR}/src/server/collision
  ${CMAKE_SOURCE_DIR}/src/server/collision/Maps
  ${CMAKE_SOURCE_DIR}/src/server/collision/Models
  ${ACE_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
)

add_executable(vmap4assembler VMapAssembler.cpp)
add_dependencies(vmap4assembler casc)

target_link_libraries(vmap4assembler
  common
  g3dlib
  ${ZLIB_LIBRARIES}
)

if( UNIX )
  install(TARGETS vmap4assembler DESTINATION bin)
elseif( WIN32 )
  install(TARGETS vmap4assembler DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
