cmake_policy(VERSION 3.0.1) # We've made this work for 3.0.1.
set(SELECTED_INCLUDE_DIRECTORIES "")
get_property(CURRENT_INCLUDE_DIRECTORIES DIRECTORY ${TOP_CMAKE_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(INCLUDE_DIR ${CURRENT_INCLUDE_DIRECTORIES})
  if(NOT INCLUDE_DIR MATCHES ".*(fhicl|tbb|messagefacility|artdaq_utilities|core|extensions|plugin|demo)")
    list(APPEND SELECTED_INCLUDE_DIRECTORIES ${INCLUDE_DIR})
  endif()
endforeach()
list (REMOVE_DUPLICATES SELECTED_INCLUDE_DIRECTORIES)
list(APPEND SELECTED_INCLUDE_DIRECTORIES "${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp")
set_property(DIRECTORY ${TOP_CMAKE_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES ${SELECTED_INCLUDE_DIRECTORIES})

get_property(CURRENT_INCLUDE_DIRECTORIES DIRECTORY ${TOP_CMAKE_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(INCLUDE_DIR ${CURRENT_INCLUDE_DIRECTORIES})
    message(STATUS "include='${INCLUDE_DIR}'")
endforeach()

set(FHICLCPP artdaq-database_Fhiclcpp)

find_path(BUILTIN_FHICLCPP_DIR ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp)

if ( (NOT BUILTIN_FHICLCPP_DIR) OR (NOT EXISTS ${BUILTIN_FHICLCPP_DIR}))
    message("Downloading fhicl-cpp source")

    execute_process(COMMAND rm -rf ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp
		    WORKING_DIRECTORY  ${TOP_CMAKE_SOURCE_DIR}/built-in)
    
    execute_process(COMMAND git clone http://cdcvs.fnal.gov/projects/fhicl-cpp ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp
                    WORKING_DIRECTORY  ${TOP_CMAKE_SOURCE_DIR}/built-in)

    execute_process(COMMAND  git checkout tags/v4_05_01 
		    WORKING_DIRECTORY  ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp)
		    
    execute_process(COMMAND  git apply ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp.patch
                    WORKING_DIRECTORY  ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp)                    
endif()

add_subdirectory(built-in)
