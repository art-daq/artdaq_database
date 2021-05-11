# ======================================================================
#  artdaq-database main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/artdaq-database/ups/setup_for_development
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -Dqualifier:STRING=$CETPKG_QUAL
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package
# ======================================================================

# use cmake 3.17 or later
cmake_minimum_required (VERSION 3.17)

project(artdaq_database VERSION v1_05_00)

SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()

set(TOP_CMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TOP_CMAKE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_MODULE_PATH
    $ENV{ARTDAQ_UTILITIES_DIR}/Modules
    $ENV{CETBUILDTOOLS_DIR}/Modules
    ${CMAKE_MODULE_PATH}
    ${TOP_CMAKE_SOURCE_DIR}/Modules
    ${TOP_CMAKE_SOURCE_DIR}/built-in)

include(CetCMakeEnv)

cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT WERROR NO_UNDEFINED EXTRA_FLAGS -pedantic )

IF (SANITIZE_ADDRESS)
  string(TOUPPER ${SANITIZE_ADDRESS} SANITIZE_ADDRESS_UC )
  if( ${SANITIZE_ADDRESS_UC} MATCHES "TRUE" )
    MESSAGE (STATUS "Enable  -fsanitize=address")
    cet_add_compiler_flags(-fsanitize=address)
  endif()
ENDIF()


cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_boost  ( v1_66_0a )
find_ups_product( cetbuildtools v7_15_01 )
find_ups_product( cetpkgsupport v1_14_01 )
find_ups_product( TRACE v3_15_09 )
find_ups_product( swig v4_0_0 )

# Debug streamer.
string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC)
if(BTYPE_UC STREQUAL DEBUG)
  add_definitions(-DDEBUGME)
endif()

# buil-in libraries
include(BuiltinMONGOC)
include(BuiltinMONGOCXX)
include(BuiltinFHICLCPP)
include(BuiltinCETLIB)
include(BuiltinCETLIBEXCEPT)
#include(BuiltinPARAMICO)
add_subdirectory(built-in)

# Build Node Addons
#include(NodeJSAddon)

# MongoDB Libraries
set(BSONCLIB bsonc)
set(MONGOCLIB mongoc)
set(BSONCXXLIB bsoncxx)
set(MONGOCXXLIB mongocxx)

#find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_LIB} )
#find_library(MONGOCXXLIB NAMES mongocxx HINTS $ENV{MONGODB_LIB} )
#find_library(BSONCLIB NAMES bsonc HINTS $ENV{MONGODB_LIB} )
#find_library(MONGOCLIB NAMES mongoc HINTS $ENV{MONGODB_LIB} )

# testing
SET(RUN_TESTS $ENV{RUN_TESTS})
IF(RUN_TESTS)
  string(TOUPPER ${RUN_TESTS} RUN_TESTS_UC )
  if( ${RUN_TESTS_UC} MATCHES "TRUE" )
    MESSAGE (STATUS "Enable tests")
    add_definitions(-DTESTBUILD)
    add_subdirectory(test)
  endif()
ENDIF()

install(DIRECTORY test/testdata DESTINATION ${product}/${version})

# source
add_subdirectory(artdaq-database)

# doc- Documentation
#add_subdirectory(doc)

add_dependencies(mongo-cxx-driver mongo-c-driver )
add_dependencies(artdaq-database_StorageProviders_MongoDB mongo-cxx-driver)


# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
