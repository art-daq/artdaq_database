cmake_policy(VERSION 3.0.1) # We've made this work for 3.0.1.
cmake_policy(SET CMP0078 OLD)

include(CetParseArgs)

set(CAN_BUILD true)

if(NOT EXISTS "$ENV{PYTHON_DIR}")
    message("Directory \"$ENV{PYTHON_DIR}\" does not exist, can't build Python addons!")
  set(CAN_BUILD false)
endif(NOT EXISTS "$ENV{PYTHON_DIR}")

if(NOT EXISTS "$ENV{SWIG_DIR}")
    message("Directory \"$ENV{SWIG_DIR}\" does not exist, can't build Python addons!")
  set(CAN_BUILD false)
endif(NOT EXISTS "$ENV{SWIG_DIR}")


if(CAN_BUILD)
  #find_ups_product(swig v3)
  #include(FindSWIG)

  FIND_PACKAGE(SWIG REQUIRED) 
  INCLUDE(${SWIG_USE_FILE})
  
  FIND_PACKAGE(PythonLibs)  
  INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

  find_ups_product(python v2_7_11)
  INCLUDE_DIRECTORIES($ENV{PYTHON_INCLUDE})
endif(CAN_BUILD)

macro (create_python_addon)
    if(CAN_BUILD)
        set(cet_file_list "")
        set(create_python_addon_usage "USAGE: create_python_addon( [ADDON_NAME <addon name>] [LIBRARIES <library list>] [INCLUDES <include directories>])")
        #message(STATUS "create_python_addon debug: called with ${ARGN} from ${CMAKE_CURRENT_SOURCE_DIR}")
        cet_parse_args( PIA "ADDON_NAME;LIBRARIES;INCLUDES" "" ${ARGN})
        # there are no default arguments
        if( PIA_DEFAULT_ARGS )
            message(FATAL_ERROR  " undefined arguments ${PIA_DEFAULT_ARGS} \n ${create_python_addon_usage}")
        endif()

    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-register")

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wno-bad-function-cast -Wno-string-conversion")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-bad-function-cast -Wno-string-conversion")
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wno-cast-function-type -Wno-stringop-truncation")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cast-function-type -Wno-stringop-truncation")
    endif()
    
    file(GLOB PIA_SOURCES  *.i)
    file(GLOB LIB_SOURCES  *.cpp)

    set_source_files_properties (${PIA_SOURCES} PROPERTIES CPLUSPLUS ON)

    list(APPEND PIA_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_INCLUDE_PATH}) 

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

    set (UseSWIG_TARGET_NAME_PREFERENCE STANDARD)
 
    swig_add_library(${PIA_ADDON_NAME} LANGUAGE python SOURCES ${PIA_SOURCES} ${LIB_SOURCES})
    TARGET_LINK_LIBRARIES (${PIA_ADDON_NAME} ${PIA_LIBRARIES})

    
    set(PIA_ADDON_LIBNAME _${PIA_ADDON_NAME})

    install (FILES ${TOP_CMAKE_BINARY_DIR}/lib/${PIA_ADDON_LIBNAME}.so 
      PERMISSIONS OWNER_EXECUTE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
      DESTINATION ${flavorqual_dir}/python/)
    
    install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${PIA_ADDON_NAME}.py DESTINATION ${flavorqual_dir}/python/)

    add_custom_command(TARGET ${PIA_ADDON_NAME} POST_BUILD 
      COMMAND echo "**** Exports for ${TOP_CMAKE_BINARY_DIR}/../lib/${PIA_ADDON_LIBNAME}.so"
      COMMAND echo "**** BEGIN"
      COMMAND /usr/bin/nm ${TOP_CMAKE_BINARY_DIR}/lib/${PIA_ADDON_LIBNAME}.so | /bin/egrep -e \"^[a-f0-9]{1,16} [T]\" | /usr/bin/c++filt  
      COMMAND echo "**** END" )

    else(CAN_BUILD)
        message("Compatible version of Swig found. NOT building ${PIA_ADDON_NAME}")
    endif(CAN_BUILD)
endmacro()
