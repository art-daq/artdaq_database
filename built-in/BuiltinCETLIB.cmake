cmake_policy(VERSION 3.0.1) # We've made this work for 3.0.1.
set(SELECTED_INCLUDE_DIRECTORIES "")
get_property(CURRENT_INCLUDE_DIRECTORIES DIRECTORY ${TOP_CMAKE_BINARY_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(INCLUDE_DIR ${CURRENT_INCLUDE_DIRECTORIES})
  if(NOT INCLUDE_DIR MATCHES ".*(cetlib|tbb|messagefacility|artdaq_utilities|core|extensions|plugin|demo)")
    list(APPEND SELECTED_INCLUDE_DIRECTORIES ${INCLUDE_DIR})
  endif()
endforeach()
list (REMOVE_DUPLICATES SELECTED_INCLUDE_DIRECTORIES)
list(APPEND SELECTED_INCLUDE_DIRECTORIES "${TOP_CMAKE_BINARY_DIR}/built-in/cetlib")
set_property(DIRECTORY ${TOP_CMAKE_BINARY_DIR} PROPERTY INCLUDE_DIRECTORIES ${SELECTED_INCLUDE_DIRECTORIES})

get_property(CURRENT_INCLUDE_DIRECTORIES DIRECTORY ${TOP_CMAKE_BINARY_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(INCLUDE_DIR ${CURRENT_INCLUDE_DIRECTORIES})
    message(STATUS "include='${INCLUDE_DIR}'")
endforeach()

if ( NOT EXISTS ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib )
    message("Downloading cetlib source")

    execute_process(COMMAND git clone --branch v3_17_00 https://github.com/art-framework-suite/cetlib.git
       WORKING_DIRECTORY  ${TOP_CMAKE_BINARY_DIR}/built-in
  RESULT_VARIABLE STATUS
  OUTPUT_VARIABLE OUTPUT1 )
  
if(STATUS AND NOT STATUS EQUAL 0)
  message(STATUS "FAILED: ${STATUS}")
else()
  message(STATUS "SUCCESS: ${OUTPUT1}")
endif()
#    execute_process(COMMAND  git apply ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib-v3_03_00b.patch
#                    WORKING_DIRECTORY  ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib)
endif()

include_directories(${TOP_CMAKE_BINARY_DIR}/built-in/cetlib)


#cet_add_compiler_flags(-fvisibility=hidden)

IF(CETLIB_IS_STATIC)
  MESSAGE (STATUS "Making cetlib static")
  
  set(SOURCES
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/canonical_number.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/canonical_string.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/include.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/includer.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/filepath_maker.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/base_converter.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/search_path.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/filesystem.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/getenv.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/split_by_regex.cc
    )

  set(LIBRARIES  
    cetlib_except
    Boost::filesystem
    Boost::system
    )

  include_directories( ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib )
    
  add_library(${CETLIB_TARGET} OBJECT ${SOURCES})
  set_property(TARGET ${CETLIB_TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
  target_link_libraries(${CETLIB_TARGET} ${LIBRARIES})  

else()
  cet_make_library(cetlib
    SOURCE
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/canonical_number.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/canonical_string.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/include.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/includer.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/filepath_maker.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/base_converter.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/search_path.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/filesystem.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/getenv.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib/cetlib/split_by_regex.cc
    LIBRARIES
    cetlib_except
    Boost::filesystem
    Boost::system
    WITH_STATIC_LIBRARY
  ) 
ENDIF()