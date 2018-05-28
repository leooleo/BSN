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
# Set linking libraries to successfully link test suites and binaries.
SET (LIBRARIES ${OPENDAVINCI_LIBRARIES}
                ${BSN_LIBRARIES})


############################################################################
# Compiling tests

file(GLOB testheader
"${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
file(GLOB includesmodule
"${DIR_ONE_ABOVE}/include/include/bsn/entity/*.hpp"
)
find_package(CxxTest)
if(CXXTEST_FOUND)
    include_directories(${CXXTEST_INCLUDE_DIR} ${includesmodule}) 
    enable_testing()

    CXXTEST_ADD_TEST(runner runner.cc 
                ${testheader})
    target_link_libraries(runner ${BSN_LIBRARIES})
endif() 

###########################################################################
# Build this project.
FILE(GLOB_RECURSE thisproject-sources "${CMAKE_CURRENT_SOURCE_DIR}/build/*.cc")
# ADD_LIBRARY (${PROJECT_NAME}lib-static STATIC ${thisproject-sources})
# ADD_EXECUTABLE (${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/build/${PROJECT_NAME}.cc")
# TARGET_LINK_LIBRARIES (${PROJECT_NAME} ${PROJECT_NAME}lib-static ${LIBRARIES}) 

###########################################################################
# Install this project.
INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin COMPONENT runner)