# ------------------------------------------------------------------------------------------------ #
#  coveralls-cmake                                                                                 #
# ------------------------------------------------------------------------------------------------ #

function (addCoverallsCMake)
  if(DEFINED COVERALLS_CMAKE_PATH)
    message(STATUS "Coveralls-cmake is already available")
  else()
    checkout_external_project(coveralls-cmake https://github.com/JoakimSoderberg/coveralls-cmake.git master)
    set(COVERALLS_CMAKE_PATH "${CMAKE_BINARY_DIR}/coveralls-cmake/src/coveralls-cmake/cmake" PARENT_SCOPE)
  endif()
endfunction()
