
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #######

cmake_minimum_required (VERSION 3.1)
project (rastr)

set(RASTR_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(RASTR_BUILD_DIR ${CMAKE_BINARY_DIR})
set(RASTR_MODULES_DIR ${RASTR_ROOT_DIR}/modules)

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
  set(IS_RASTR ON)
else()
  set(IS_RASTR OFF)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

if(IS_RASTR)
  include(colors)
endif()

set( CMAKE_CXX_STANDARD 11 )

# Options ---------------------------------------------------------------------------------------- #

option( BUILD_SHARED_LIBS "Build as shared library" OFF )
option( BUILD_STATIC_LIBS "Build as static library" ON  )

option( RASTR_FORCE_BUILD_DEPENDENCIES   "Force download and build dependencies" OFF)

option( RASTR_TOOLS         "Build rastr tools"     ${IS_RASTR} )
option( RASTR_TEST          "Build rastr tests"     ${IS_RASTR} )
option( RASTR_DOCS          "Build rastr docs"      ${IS_RASTR} )

option( COVERALLS           "Turn on Coveralls support" OFF )
option( COVERALLS_UPLOAD    "Upload the generated Coveralls json" ON )

option( RASTR_TEST_ALL      "Generate test ouput for successful tests as well" OFF )
option( RASTR_TEST_JUNIT    "Generate a JUNIT xml test report" OFF )
option( RASTR_INSTALL_PATH  "/usr/local/include/")


# Dependencies ----------------------------------------------------------------------------------- #

if(${RASTR_TOOLS} OR ${RASTR_TEST})

  set(Boost_USE_STATIC_LIBS OFF)
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
  find_package(Boost 1.63.0 COMPONENTS system filesystem REQUIRED)
  if(UNIX AND NOT ${RASTR_FORCE_BUILD_DEPENDENCIES})
    include(FindPNG REQUIRED)
  else()
    set(PNG_TESTS OFF)
    add_subdirectory(${RASTR_MODULES_DIR}/libpng)
  endif()

endif()

set(CATCH_INCLUDE_DIRS ${RASTR_MODULES_DIR}/catch2/single_include )
set(CLARA_INCLUDE_DIRS ${RASTR_MODULES_DIR}/clara/single_include )


# Version ---------------------------------------------------------------------------------------- #

string(TIMESTAMP RASTR_BUILD_DATE "%Y-%m-%d")


set (RASTR_VERSION_MAJOR 0)
set (RASTR_VERSION_MINOR 1)
set (RASTR_VERSION_MICRO 0)
set (RASTR_VERSION_STRING "${RASTR_VERSION_MAJOR}.${RASTR_VERSION_MINOR}.${RASTR_VERSION_MICRO}")

if($ENV{APPVEYOR})
  set (RASTR_BUILD_HOST "Appveyor")
  set (RASTR_BUILD_NUMBER ".$ENV{APPVEYOR_BUILD_NUMBER}")
  set (RASTR_GIT_BRANCH $ENV{APPVEYOR_REPO_BRANCH})
  execute_process(COMMAND appveyor UpdateBuild -Version ${RASTR_VERSION_STRING})
elseif($ENV{TRAVIS})
  set (RASTR_BUILD_HOST "Travis-CI")
  set (RASTR_BUILD_NUMBER ".$ENV{TRAVIS_BUILD_NUMBER}")
  set (RASTR_GIT_BRANCH $ENV{TRAVIS_BRANCH})
else()
  set (RASTR_BUILD_HOST "a local machine")
  set (RASTR_BUILD_NUMBER "")
  execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${RASTR_ROOT_DIR}
    OUTPUT_VARIABLE RASTR_GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()

if(${RASTR_GIT_BRANCH} STREQUAL "master")
  set(RASTR_BUILD_TYPE "release version")
  set (RASTR_VERSION_STRING "${RASTR_VERSION_STRING}${RASTR_BUILD_NUMBER}")
elseif(${RASTR_GIT_BRANCH} STREQUAL "develop")
  set(RASTR_BUILD_TYPE "development version")
  set (RASTR_VERSION_STRING "${RASTR_VERSION_STRING}${RASTR_BUILD_NUMBER}-dev")
else()
  set(RASTR_BUILD_TYPE "development version from feature branch: ${GIT_BRANCH}")
  set (RASTR_VERSION_STRING "${RASTR_VERSION_STRING}${RASTR_BUILD_NUMBER}-fb")
endif()

configure_file (
  "${RASTR_ROOT_DIR}/support/rastr.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/rastr.h"
)

if($ENV{APPVEYOR})
  execute_process(COMMAND appveyor UpdateBuild -Version ${RASTR_VERSION_STRING})
  set(ENV{APPVEYOR_BUILD_VERSION} "${RASTR_VERSION_STRING}")
endif()

if(IS_RASTR)
  message( "${White}")
  message( "${White}    ,-.  ,-.  ,-.  |-  ,-. ")
  message( "${White}    |    ,-|  `-.  |   |   ")
  message( "${White}    '    `-^  `-'  `'  '   ")
  set(RASTR_BUILD_DESCRIPTION "${Magenta}a tiny graphic library${ColourReset}")
  message(  "    ${RASTR_BUILD_DESCRIPTION} ")
  message(  "                  v. ${RASTR_VERSION_STRING}")
  message(  " ")
  message(  " ")
  message(  "    ${RASTR_BUILD_TYPE} built on ${RASTR_BUILD_HOST} (${RASTR_BUILD_DATE})")
  message(  " ")
  message(  " ")
endif()

# Source files ----------------------------------------------------------------------------------- #

set(
  inc_rastr_INCLUDES
    inc/rastr/Canvas.hpp
    inc/rastr/CanvasMonochrome.hpp
    inc/rastr/CanvasGrayscale8.hpp
    inc/rastr/CanvasRGB565.hpp
    inc/rastr/CanvasRGB888.hpp
    inc/rastr/Color.hpp
    inc/rastr/StaticCanvas.hpp
    inc/rastr/DynamicCanvas.hpp
    inc/rastr/Types.hpp
)
source_group("inc\\rastr"  FILES ${inc_rastr_INCLUDES})

set(
  rastr_ALL
    ${inc_rastr_INCLUDES}
)

add_library( rastr INTERFACE )
target_include_directories( rastr INTERFACE inc "${CMAKE_CURRENT_BINARY_DIR}" )

# OS X and Linux --------------------------------------------------------------------------------- #

if( UNIX )

  if(${IS_RASTR})

    install (DIRECTORY  inc/rastr        DESTINATION "${RASTR_INSTALL_PATH}")

    install (
      FILES        "${CMAKE_CURRENT_BINARY_DIR}/rastr.h"
      DESTINATION  "${RASTR_INSTALL_PATH}/"
    )
  endif()

endif()


# Tools ------------------------------------------------------------------------------------------ #

if(${RASTR_TOOLS})
  add_subdirectory(tools)
endif()


# Unit tests ------------------------------------------------------------------------------------- #

if(${RASTR_TEST})
  enable_testing(true)
  add_subdirectory(test)
endif()


# Coveralls -------------------------------------------------------------------------------------- #

if(COVERALLS)
  include(Coveralls)
  message("Coverage build enabled")
  addCoverallsCMake()
  list(APPEND CMAKE_MODULE_PATH "${COVERALLS_CMAKE_PATH}")
  include(Coveralls)
  coveralls_turn_on_coverage()
  coveralls_setup(
    "${RASTR_ALL}"
    ${COVERALLS_UPLOAD}
    "${COVERALLS_CMAKE_PATH}"
  )
endif()

# If Doxygen is not available, just display a warning and skip the documentation target
if(${RASTR_DOCS})
  find_package(Doxygen)
  if(NOT DOXYGEN_FOUND)
    message(WARNING "Doxygen has not been found, the documentation will not be created.")
    set(RASTR_DOCS OFF)
  endif()
endif()


# Arduino/Teensy specific ------------------------------------------------------------------------ #

if(${IS_RASTR} AND UNIX)
add_custom_target( CreateArduinoLibrary
  command ${CMAKE_SOURCE_DIR}/arduinify.sh ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
)
configure_file (
  "${PROJECT_SOURCE_DIR}/support/arduino/library.properties.in"
  "${PROJECT_BINARY_DIR}/library.properties"
)
endif()


# Documentation (doxygen) ------------------------------------------------------------------------ #

if(${RASTR_DOCS})

  if(NOT DOXYGEN_FOUND)

    message(FATAL_ERROR "Doxygen is needed to build the documentation.")

  else()

    set( doxyfile_in          ${RASTR_ROOT_DIR}/support/doxygen/Doxyfile.in )
    set( doxyfile             "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"                  )

    set( doxy_project_name    ${PROJECT_NAME}                                         )
    set( doxy_input_folder    ${PROJECT_SOURCE_DIR}/inc                               )
    set( doxy_output_root     ${PROJECT_SOURCE_DIR}/docs                              )
    set( doxy_readme_md       ${PROJECT_SOURCE_DIR}/README.md                         )

    set( doxy_html_root       ${PROJECT_SOURCE_DIR}/support/doxygen                   )

    set( doxy_project_version "v. ${RASTR_VERSION_MAJOR}.${RASTR_VERSION_MINOR}.${RASTR_VERSION_MICRO}")

    configure_file( ${doxyfile_in} ${doxyfile} @ONLY )

    add_custom_target(
      doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )

  endif()
endif()
