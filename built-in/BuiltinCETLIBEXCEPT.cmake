cmake_policy(VERSION 3.0.1) # We've made this work for 3.0.1.
set(SELECTED_INCLUDE_DIRECTORIES "")
get_property(CURRENT_INCLUDE_DIRECTORIES DIRECTORY ${TOP_CMAKE_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(INCLUDE_DIR ${CURRENT_INCLUDE_DIRECTORIES})
  if(NOT INCLUDE_DIR MATCHES ".*(cetlib_except_except|tbb|messagefacility|artdaq_utilities|core|extensions|plugin|demo)")
    list(APPEND SELECTED_INCLUDE_DIRECTORIES ${INCLUDE_DIR})
  endif()
endforeach()
list (REMOVE_DUPLICATES SELECTED_INCLUDE_DIRECTORIES)
list(APPEND SELECTED_INCLUDE_DIRECTORIES "${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib_except_except")
set_property(DIRECTORY ${TOP_CMAKE_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES ${SELECTED_INCLUDE_DIRECTORIES})

get_property(CURRENT_INCLUDE_DIRECTORIES DIRECTORY ${TOP_CMAKE_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(INCLUDE_DIR ${CURRENT_INCLUDE_DIRECTORIES})
    message(STATUS "include='${INCLUDE_DIR}'")
endforeach()

set(CETLIB_EXCEPT artdaq-database_cetlibexcept)

if ( NOT EXISTS ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib_except )
    message("Downloading cetlib_except source")
    execute_process(COMMAND rm -rf ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib_except
        WORKING_DIRECTORY  ${TOP_CMAKE_SOURCE_DIR}/built-in)

    execute_process(COMMAND git clone https://cdcvs.fnal.gov/projects/cetlib_except ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib_except
        WORKING_DIRECTORY  ${TOP_CMAKE_SOURCE_DIR}/built-in)

    execute_process(COMMAND  git checkout tags/v1_02_00
        WORKING_DIRECTORY  ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib_except)

#    execute_process(COMMAND  git apply ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib_except-v1_02_00.patch
#                    WORKING_DIRECTORY  ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib_except)
endif()

include_directories(built-in/cetlib_except)
