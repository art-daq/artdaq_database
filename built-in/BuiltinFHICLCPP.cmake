cmake_policy(VERSION 3.0.1) # We've made this work for 3.0.1.
set(SELECTED_INCLUDE_DIRECTORIES "")
get_property(CURRENT_INCLUDE_DIRECTORIES DIRECTORY ${TOP_CMAKE_BINARY_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(INCLUDE_DIR ${CURRENT_INCLUDE_DIRECTORIES})
  if(NOT INCLUDE_DIR MATCHES ".*(fhicl|tbb|messagefacility|artdaq_utilities|core|extensions|plugin|demo)")
    list(APPEND SELECTED_INCLUDE_DIRECTORIES ${INCLUDE_DIR})
  endif()
endforeach()
list (REMOVE_DUPLICATES SELECTED_INCLUDE_DIRECTORIES)
list(APPEND SELECTED_INCLUDE_DIRECTORIES "${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp")
set_property(DIRECTORY ${TOP_CMAKE_BINARY_DIR} PROPERTY INCLUDE_DIRECTORIES ${SELECTED_INCLUDE_DIRECTORIES})

get_property(CURRENT_INCLUDE_DIRECTORIES DIRECTORY ${TOP_CMAKE_BINARY_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(INCLUDE_DIR ${CURRENT_INCLUDE_DIRECTORIES})
    message(STATUS "include='${INCLUDE_DIR}'")
endforeach()


if ( NOT EXISTS ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp )
    message("Downloading fhicl-cpp source")

    execute_process(COMMAND git clone --branch v4_18_00 https://github.com/art-framework-suite/fhicl-cpp.git  WORKING_DIRECTORY  ${TOP_CMAKE_BINARY_DIR}/built-in
  RESULT_VARIABLE STATUS
  OUTPUT_VARIABLE OUTPUT1 )
if(STATUS AND NOT STATUS EQUAL 0)
  message(STATUS "FAILED: ${STATUS}")
else()
  message(STATUS "SUCCESS: ${OUTPUT1}")
endif()

    execute_process(COMMAND  git apply ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp-v4_18_00.patch
         WORKING_DIRECTORY  ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp
  RESULT_VARIABLE STATUS
  OUTPUT_VARIABLE OUTPUT2 )
if(STATUS AND NOT STATUS EQUAL 0)
  message(STATUS "FAILED: ${STATUS}")
else()
  message(STATUS "SUCCESS: ${OUTPUT2}")
endif()
endif()

include_directories(${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp)


#cet_add_compiler_flags(-fvisibility=hidden)


IF(FHICLCPP_IS_STATIC)
  MESSAGE (STATUS "Making FHiCL static")
  
  set(SOURCES
    ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp/fhiclcpp/exception.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp/fhiclcpp/extended_value.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp/fhiclcpp/intermediate_table.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp/fhiclcpp/parse.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp/fhiclcpp/parse_shims_opts.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp/fhiclcpp/Protection.cc
    )

  set(LIBRARIES  
    cetlib
    cetlib_except
    Boost::headers
    )
    
  include_directories( ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp )
    
  add_library(${FHICLCPP_TARGET} OBJECT ${SOURCES})
  set_property(TARGET ${FHICLCPP_TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
  target_link_libraries(${FHICLCPP_TARGET} ${LIBRARIES})  

else()
  cet_make(fhiclcpp
    SOURCE
    ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp/fhiclcpp/exception.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp/fhiclcpp/extended_value.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp/fhiclcpp/intermediate_table.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp/fhiclcpp/parse.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp/fhiclcpp/parse_shims_opts.cc
    ${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp/fhiclcpp/Protection.cc
    LIBRARIES
    cetlib
    cetlib_except
    WITH_STATIC_LIBRARY
  ) 
ENDIF()
