cmake_policy(VERSION 3.0.1) # We've made this work for 3.0.1.
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

#  find_ups_product(python v2_7_11)
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
            message(FATAL_ERROR  " undefined arguments ${CNA_DEFAULT_ARGS} \n ${create_python_addon_usage}")
        endif()

    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")

    file(GLOB PIA_SOURCES  *.i)
    file(GLOB LIB_SOURCES  *.cpp)

    set_source_files_properties (${PIA_SOURCES} PROPERTIES CPLUSPLUS ON)

    list(APPEND PIA_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_INCLUDE_PATH}) 

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

    swig_add_module (${PIA_ADDON_NAME} python ${PIA_SOURCES} ${LIB_SOURCES})
    swig_link_libraries (${PIA_ADDON_NAME} ${PIA_LIBRARIES})

    set(PIA_ADDON_LIBNAME _${PIA_ADDON_NAME})

    install (FILES ${LIBRARY_OUTPUT_PATH}/${PIA_ADDON_LIBNAME}.so DESTINATION ${flavorqual_dir}/lib/)
    install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${PIA_ADDON_NAME}.py DESTINATION ${flavorqual_dir}/python/)

    add_custom_command(TARGET ${PIA_ADDON_LIBNAME} POST_BUILD 
      COMMAND echo "**** Exports for ${LIBRARY_OUTPUT_PATH}/${PIA_ADDON_LIBNAME}.so"
      COMMAND echo "**** BEGIN"
      COMMAND /usr/bin/nm ${LIBRARY_OUTPUT_PATH}/${PIA_ADDON_LIBNAME}.so | /bin/egrep -e \"^[a-f0-9]{1,16} [T]\" | /usr/bin/c++filt  
      COMMAND echo "**** END" )

    else(CAN_BUILD)
        message("Compatible version of Swig found. NOT building ${PIA_ADDON_NAME}")
    endif(CAN_BUILD)
endmacro()