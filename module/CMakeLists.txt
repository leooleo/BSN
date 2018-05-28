CMAKE_MINIMUM_REQUIRED (VERSION 3.5)

PROJECT (runner)

###########################################################################
# Set the search path for .cmake files.
SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake.Modules" ${CMAKE_MODULE_PATH})

get_filename_component(DIR_ONE_ABOVE ../ ABSOLUTE)

# Add a local CMake module search path dependent on the desired installation destination.
# Thus, artifacts from the complete source build can be given precendence over any installed versions.
IF(UNIX)
    SET (CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/share/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})
ENDIF()
IF(WIN32)
    SET (CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/CMake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})
ENDIF()

###########################################################################
# Include flags for compiling.
INCLUDE (${CMAKE_CURRENT_SOURCE_DIR}/cmake.Modules/CompileFlags.cmake)

###########################################################################
# Find OpenDaVINCI.
SET(OPENDAVINCI_DIR "${CMAKE_INSTALL_PREFIX}")
FIND_PACKAGE (OpenDaVINCI REQUIRED)

# Find BSN.
SET(BSN_DIR "${CMAKE_INSTALL_PREFIX}")
FIND_PACKAGE (BSN REQUIRED)

###########################################################################
# Finding the CxxTest

file(GLOB testheader
"${CMAKE_SOURCE_DIR}/test/*.hpp"
)
add_subdirectory(bodyhub)
add_subdirectory(sensornode)
file(GLOB includesmodule
"${CMAKE_SOURCE_DIR}/*/include/"
)
find_package(CxxTest)
if(CXXTEST_FOUND)
    include_directories(${CXXTEST_INCLUDE_DIR} ${includesmodule})
    enable_testing()

    CXXTEST_ADD_TEST(runner runner.cc 
                ${testheader})
    target_link_libraries(runner)
endif() 

###########################################################################
# Build this project.
FILE(GLOB_RECURSE thisproject-sources "${CMAKE_CURRENT_SOURCE_DIR}/build/*.cc")

###########################################################################
# Install this project.
INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin COMPONENT runner)