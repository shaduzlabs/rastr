
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #######

# Tools ------------------------------------------------------------------------------------------ #

include(FindFreetype)
if (NOT FREETYPE_FOUND)
  message(FATAL_ERROR "FreeType is required and could not be found.")
endif()

set( src_rastr_png_SRCS rastr-png.cpp )
add_executable( rastr-png ${src_rastr_png_SRCS} )
target_include_directories(rastr-png PRIVATE ${Boost_INCLUDE_DIRS} ${CLARA_INCLUDE_DIRS})
target_link_libraries(rastr-png PUBLIC rastr png ${Boost_LIBRARIES})

set( src_rastr_ttf_SRCS rastr-ttf.cpp )
add_executable( rastr-ttf ${src_rastr_ttf_SRCS} )
target_include_directories(rastr-ttf PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(rastr-ttf PRIVATE ${FREETYPE_INCLUDE_DIRS} ${CLARA_INCLUDE_DIRS})
target_link_libraries(rastr-ttf PUBLIC rastr ${Boost_LIBRARIES} ${FREETYPE_LIBRARIES})
