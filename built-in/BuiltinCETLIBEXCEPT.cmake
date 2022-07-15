cmake_policy(VERSION 3.0.1) # We've made this work for 3.0.1.
set(SELECTED_INCLUDE_DIRECTORIES "")
get_property(CURRENT_INCLUDE_DIRECTORIES DIRECTORY ${TOP_CMAKE_BINARY_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(INCLUDE_DIR ${CURRENT_INCLUDE_DIRECTORIES})
  if(NOT INCLUDE_DIR MATCHES ".*(cetlib_except|tbb|messagefacility|artdaq_utilities|core|extensions|plugin|demo)")
    list(APPEND SELECTED_INCLUDE_DIRECTORIES ${INCLUDE_DIR})
  endif()
endforeach()
list (REMOVE_DUPLICATES SELECTED_INCLUDE_DIRECTORIES)
list(APPEND SELECTED_INCLUDE_DIRECTORIES "${TOP_CMAKE_BINARY_DIR}/built-in/cetlib_except")
set_property(DIRECTORY ${TOP_CMAKE_BINARY_DIR} PROPERTY INCLUDE_DIRECTORIES ${SELECTED_INCLUDE_DIRECTORIES})

get_property(CURRENT_INCLUDE_DIRECTORIES DIRECTORY ${TOP_CMAKE_BINARY_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(INCLUDE_DIR ${CURRENT_INCLUDE_DIRECTORIES})
    message(STATUS "include='${INCLUDE_DIR}'")
endforeach()


if ( NOT EXISTS ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib_except )
    message("Downloading cetlib_except source")

    execute_process(COMMAND git clone --branch v1_07_02 https://github.com/art-framework-suite/cetlib-except.git cetlib_except WORKING_DIRECTORY  ${TOP_CMAKE_BINARY_DIR}/built-in
  RESULT_VARIABLE STATUS
  OUTPUT_VARIABLE OUTPUT1 )
    
if(STATUS AND NOT STATUS EQUAL 0)
  message(STATUS "FAILED: ${STATUS}")
else()
  message(STATUS "SUCCESS: ${OUTPUT1}")
endif()

#    execute_process(COMMAND  git apply ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib_except-v1_02_00.patch
#                    WORKING_DIRECTORY  ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib_except)
endif()

include_directories(${TOP_CMAKE_BINARY_DIR}/built-in/cetlib_except)

#cet_add_compiler_flags(-fvisibility=hidden)
IF(CETLIBEXCEPT_IS_STATIC)
  MESSAGE (STATUS "Making cetlib_except static")
  
  set(SOURCES
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib_except/cetlib_except/exception.cc
    )
    
  set(LIBRARIES  
    )
    
  add_library(${CETLIB_EXCEPT_TARGET} OBJECT ${SOURCES})
  set_property(TARGET ${CETLIB_EXCEPT_TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
  target_link_libraries(${CETLIB_EXCEPT_TARGET} ${LIBRARIES})  

else()
  cet_make(
    LIBRARY_NAME 
    cetlib_except
    LIBRARIES
    WITH_STATIC_LIBRARY
    SUBDIRS
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib_except/cetlib_except
    EXCLUDE
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib_except/cetlib_except/demangle.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/cetlib_except/cetlib_except/exception_collector.cc
  ) 
ENDIF()