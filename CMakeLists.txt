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

project(artdaq-database VERSION 3.03.01)

set(TOP_CMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TOP_CMAKE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

include(CetCMakeEnv)
include(SetCompilerFlags)
include(InstallSource)
include(InstallHeaders)
include(InstallFhicl)

cet_cmake_env()
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Find our own CMake modules, and tell everyone else where to find them.
cet_cmake_module_directories(Modules BINARY)
list(PREPEND CMAKE_MODULE_PATH ${TOP_CMAKE_SOURCE_DIR}/built-in)

cet_set_compiler_flags(DIAGS VIGILANT WERROR NO_UNDEFINED EXTRA_FLAGS -pedantic )

IF (SANITIZE_ADDRESS)
  string(TOUPPER ${SANITIZE_ADDRESS} SANITIZE_ADDRESS_UC )
  if( ${SANITIZE_ADDRESS_UC} MATCHES "TRUE" )
    MESSAGE (STATUS "Enable  -fsanitize=address")
			add_compile_options(-fsanitize=address -Wno-unused-command-line-argument)
			add_link_options(-lasan)
  endif()
ENDIF()

#cet_report_compiler_flags()

# Debug streamer.
string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC)
if(BTYPE_UC STREQUAL DEBUG)
  add_definitions(-DDEBUGME)
	if(DEFINED ENV{USE_ASAN})
		if($ENV{USE_ASAN} GREATER 0)
			message("!!! Address Sanitizer Activated!!!")
			add_compile_options(-fsanitize=address -Wno-unused-command-line-argument)
			add_link_options(-lasan)
		endif()
	endif()
	if(DEFINED ENV{USE_TSAN})
		if($ENV{USE_TSAN} GREATER 0)
			message("!!! Thread Sanitizer Activated!!!")
			add_compile_options(-fsanitize=thread -Wno-unused-command-line-argument)
			add_link_options(-ltsan)
		endif()
	endif()
	if(DEFINED ENV{USE_UBSAN})
		if($ENV{USE_UBSAN} GREATER 0)
			message("!!! \"Undefined Behavior\" Sanitizer Activated!!!")
			add_compile_options(-fsanitize=undefined -Wno-unused-command-line-argument)
			add_link_options(-lubsan)
		endif()
	endif()
	if(DEFINED ENV{USE_GCOV})
		add_compile_options(-fprofile-arcs -ftest-coverage --coverage)
		add_link_options(--coverage)
	endif()
endif()

find_package(Boost QUIET COMPONENTS filesystem program_options system unit_test_framework REQUIRED)
find_package(TRACE REQUIRED)

find_package(cetlib 3.13.00 REQUIRED QUIET)
find_package(cetlib_except 1.07.00 REQUIRED QUIET)

# Build Node Addons
#include(NodeJSAddon)

add_definitions("-DUSE_FHICLCPP=1")

find_package(mongocxx REQUIRED EXPORT)
find_package(bsoncxx REQUIRED EXPORT)

# testing
SET(RUN_TESTS $ENV{RUN_TESTS})
SET(RUN_TESTS "TRUE")
IF(RUN_TESTS)
  string(TOUPPER ${RUN_TESTS} RUN_TESTS_UC )
  if( ${RUN_TESTS_UC} MATCHES "TRUE" )
    MESSAGE (STATUS "Enable tests")
    add_compile_definitions(TESTBUILD)
    add_subdirectory(test)
  endif()
ENDIF()

install(DIRECTORY test/testdata DESTINATION .)

# source
add_subdirectory(artdaq-database)

# doc- Documentation
#add_subdirectory(doc)

cet_cmake_config()
