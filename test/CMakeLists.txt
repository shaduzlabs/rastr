
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #######

# ------------------------------------------------------------------------------------------------ #
#  Unit tests                                                                                      #
# ------------------------------------------------------------------------------------------------ #

include_directories( ${CATCH_INCLUDE_DIRS} )

project(unit-tests)

set(
  test_SRCS
    main.cpp
)

set(
  test_src_SRCS
    src/Canvas.cpp
    src/CanvasTestFunctions.cpp
    src/CanvasTestFunctions.h
    src/CanvasTestHelpers.cpp
    src/CanvasTestHelpers.h
)

set(
  test_gfx_displays_SRCS
    gfx/displays/TextDisplay7Segments.cpp
    gfx/displays/TextDisplayGeneric.cpp
    gfx/displays/TextDisplayKompleteKontrol.cpp
    gfx/displays/NullCanvas.cpp
    gfx/displays/GDisplayMaschineMikro.cpp
    gfx/displays/GDisplayMaschineMK1.cpp
    gfx/displays/GDisplayMaschineMK2.cpp
    gfx/displays/GDisplayPush2.cpp
)

set(
  test_util_SRCS
    util/Color.cpp
)

source_group(""                  FILES ${test_SRCS})
source_group("src"               FILES ${test_src_SRCS})

set(
  Test_FILES
    ${test_SRCS}
    ${test_src_SRCS}
)

add_executable(
  ${PROJECT_NAME}
  ${Test_FILES}
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${RASTR_ROOT_DIR}/support/test-data $<TARGET_FILE_DIR:${PROJECT_NAME}>/test-data
)

if(COVERALLS)
  set_target_properties ( ${PROJECT_NAME} PROPERTIES LINK_FLAGS " -Wall -fprofile-arcs -ftest-coverage")
endif()

target_include_directories( ${PROJECT_NAME} PRIVATE ${CATCH_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR} ${RASTR_ROOT_DIR}/src)
target_link_libraries( ${PROJECT_NAME} PRIVATE rastr png)

set(CATCH_CMDARGS "")
if(RASTRL_TEST_ALL)
  set(CATCH_CMDARGS "${CATCH_CMDARGS} -s")
endif()
if(RASTR_TEST_JUNIT)
  set(CATCH_CMDARGS "${CATCH_CMDARGS} -r junit -o test-report.junit.xml")
else()
  set(CATCH_CMDARGS "${CATCH_CMDARGS} -r compact")
endif()

if(WIN32)
  separate_arguments(CATCH_CMDARGS_LIST WINDOWS_COMMAND "${CATCH_CMDARGS}")
else()
  separate_arguments(CATCH_CMDARGS_LIST UNIX_COMMAND "${CATCH_CMDARGS}")
endif()

add_test( NAME ${PROJECT_NAME} WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}> COMMAND ${PROJECT_NAME} ${CATCH_CMDARGS_LIST})

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C ${CMAKE_BUILD_TYPE} DEPENDS ${PROJECT_NAME})
