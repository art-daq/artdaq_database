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
cmake_minimum_required (VERSION 3.19 FATAL_ERROR)

find_package(cetmodules 3.08.00 REQUIRED)

project(artdaq_database VERSION 1.07.00)

set(TOP_CMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TOP_CMAKE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})


# Find our own CMake modules, and tell everyone else where to find them.
cet_cmake_module_directories(Modules BINARY)
list(PREPEND CMAKE_MODULE_PATH ${TOP_CMAKE_SOURCE_DIR}/built-in)

include(CetCMakeEnv)

cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT WERROR NO_UNDEFINED EXTRA_FLAGS -pedantic )

IF (SANITIZE_ADDRESS)
  string(TOUPPER ${SANITIZE_ADDRESS} SANITIZE_ADDRESS_UC )
  if( ${SANITIZE_ADDRESS_UC} MATCHES "TRUE" )
    MESSAGE (STATUS "Enable  -fsanitize=address")
			add_compile_options(-fsanitize=address -Wno-unused-command-line-argument)
			add_link_options(-lasan)
  endif()
ENDIF()

string(FIND ${artdaq_database_EXEC_PREFIX} "ots" OTS_FOUND)
if(${OTS_FOUND} EQUAL -1)
    MESSAGE( STATUS "Enable built-in FHICLCPP")
  set(USE_FHICLCPP TRUE)
  else()
  MESSAGE(STATUS "DISABLE built-in FHICLCPP")
endif()
#cet_report_compiler_flags()

# Debug streamer.
string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC)
if(BTYPE_UC STREQUAL DEBUG)
  add_definitions(-DDEBUGME)
endif()

find_package(Boost COMPONENTS filesystem program_options system REQUIRED)
find_package(TRACE REQUIRED)

# buil-in libraries
include(BuiltinMONGOC)
include(BuiltinMONGOCXX)
if(USE_FHICLCPP)
  add_subdirectory(built-in)
  include(BuiltinTargets)
  include(BuiltinFHICLCPP)
  include(BuiltinCETLIB)
  include(BuiltinCETLIBEXCEPT)
else()
  find_package(cetlib 3.00.00 REQUIRED)
  find_package(cetlib_except 1.07.00 REQUIRED)
endif()
#include(BuiltinPARAMICO)

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

cet_cmake_config()

