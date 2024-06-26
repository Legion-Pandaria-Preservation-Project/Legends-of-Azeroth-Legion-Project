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

# Enforce compileparameters for corebuilds under GCC
# This to stop a few silly crashes that could have been avoided IF people
# weren't doing some -O3 psychooptimizations etc.

if(CMAKE_COMPILER_IS_GNUCXX AND NOT MINGW)
  add_definitions(-fno-delete-null-pointer-checks)
endif()

if(WIN32)
  set(sources_windows
    ${CMAKE_SOURCE_DIR}/src/common/Debugging/WheatyExceptionReport.cpp
    ${CMAKE_SOURCE_DIR}/src/common/Debugging/WheatyExceptionReport.h
    ${CMAKE_SOURCE_DIR}/src/common/Platform/ServiceWin32.cpp
    ${CMAKE_SOURCE_DIR}/src/common/Platform/ServiceWin32.h
  )
endif(WIN32)

add_subdirectory(proto)
add_subdirectory(shared)
add_subdirectory(bnetserver)
add_subdirectory(game)
add_subdirectory(scripts)
add_subdirectory(newscripts)
add_subdirectory(worldserver)
