# - Find the native PNG includes and library
#
# This module defines
#  PNG_INCLUDE_DIRS, where to find png.h, etc.
#  PNG_LIBRARIES, the libraries to link against to use PNG.
#  PNG_DEFINITIONS - You should ADD_DEFINITONS(${PNG_DEFINITIONS}) before compiling code that includes png library files.
#  PNG_FOUND, If false, do not try to use PNG.
# also defined, but not for general use are
#  PNG_LIBRARIES, where to find the PNG library.
# None of the above will be defined unles zlib can be found.
# PNG depends on Zlib
#
# Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
# See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

include(FindZLIB)
set(PNG_FOUND FALSE)

if(ZLIB_FOUND)

  FIND_PATH(PNG_INCLUDE_DIRS png.h
    /usr/local/include
    /usr/include
  )

  find_library(PNG_LIBRARIES
    NAMES png libpng
    PATHS /lib /usr/lib /usr/lib64 /usr/local/lib
  )

  if (PNG_LIBRARIES AND PNG_INCLUDE_DIRS)
    set(PNG_INCLUDE_DIRS ${PNG_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR} )
    set(PNG_LIBRARIES ${PNG_LIBRARIES} ${ZLIB_LIBRARY})
    set(PNG_FOUND TRUE)
  endif (PNG_LIBRARIES AND PNG_INCLUDE_DIRS)

else (ZLIB_FOUND)

  message(FATAL_ERROR "Could not find ZLib library")

endif(ZLIB_FOUND)

if (PNG_FOUND)
  if (NOT PNG_FIND_QUIETLY)
    message(STATUS "Found PNG: ${PNG_LIBRARIES}")
  endif (NOT PNG_FIND_QUIETLY)

  add_library(png INTERFACE)
  target_link_libraries( png INTERFACE ${PNG_LIBRARIES} )
  target_include_directories( png INTERFACE ${PNG_INCLUDE_DIRS} )

else (PNG_FOUND)

  if (PNG_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find PNG library")
  endif (PNG_FIND_REQUIRED)

endif (PNG_FOUND)
