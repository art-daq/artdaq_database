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

# use cmake 2.8 or later
cmake_minimum_required (VERSION 2.8)

project(artdaq-database)
# cetbuildtools contains our cmake modules
SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()

set(CMAKE_MODULE_PATH
    $ENV{ARTDAQ_UTILITIES_DIR}/Modules
    $ENV{CETBUILDTOOLS_DIR}/Modules
	${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic
)

string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC )

IF (SANITIZE_ADDRESS)
  string(TOUPPER ${SANITIZE_ADDRESS} SANITIZE_ADDRESS_UC )
  if( ${SANITIZE_ADDRESS_UC} MATCHES "TRUE" )
    MESSAGE (STATUS "Enable  -fsanitize=address")
    cet_add_compiler_flags(-fsanitize=address)
  endif()
ENDIF()
cet_add_compiler_flags( -rdynamic -ldl)

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_boost(v1_57_0)
find_ups_product (cetbuildtools v5_04_01)
find_ups_product( artdaq_utilities v1_00_08 )
find_ups_product( cetlib v1_18_02 )
find_ups_product( cetpkgsupport v1_10_02 )
find_ups_product( fhiclcpp v3_20_01 )
find_ups_product( sqlite v3_12_02_00)
find_ups_product( mongodb v3_2_7 )
find_ups_product( TRACE v3_06_07 )
find_ups_product( swig v3_0_10 )

# Debug streamer.
string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC)
if(BTYPE_UC STREQUAL DEBUG)
  add_definitions(-DDEBUGME)
endif()


# Build Node Addons
include(NodeJSAddon)

# MongoDB Libraries
find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_LIB} )
find_library(MONGOCXXLIB NAMES mongocxx HINTS $ENV{MONGODB_LIB} )

# testing
add_subdirectory(test)

# source
add_subdirectory(artdaq-database)

# doc- Documentation
#add_subdirectory(doc)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
