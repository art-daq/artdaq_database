cmake_policy(VERSION 3.0.1) # We've made this work for 3.0.1.
cmake_policy(SET CMP0078 OLD)
cmake_policy(SET CMP0086 OLD)

include(CetParseArgs)

set(CAN_BUILD true)
if(NOT EXISTS "$ENV{SWIG_DIR}")
    message("Directory \"$ENV{SWIG_DIR}\" does not exist, can't build Node.js addons!")
  set(CAN_BUILD false)
endif(NOT EXISTS "$ENV{SWIG_DIR}")
if(NOT EXISTS "$ENV{NODEJS_DIR}")
    message("Directory \"$ENV{NODEJS_DIR}\" does not exist, can't build Node.js addons!")
  set(CAN_BUILD false)
endif(NOT EXISTS  "$ENV{NODEJS_DIR}")

if(CAN_BUILD)
  find_ups_product(swig v3)
  include(FindSWIG)
  INCLUDE(${SWIG_USE_FILE}) 
  find_ups_product(nodejs v4_5_0)
endif(CAN_BUILD)

SET(NODEJS_ADDON_DIR ${CMAKE_CURRENT_LIST_DIR})

function(create_node_package_json NODEJS_ADDON_NAME)
if(CAN_BUILD)
        configure_file (${NODEJS_ADDON_DIR}/package.json.in ${CMAKE_CURRENT_BINARY_DIR}/package.json @ONLY)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/package.json
            DESTINATION ${flavorqual_dir}/lib/node_modules/${NODEJS_ADDON_NAME} )
endif(CAN_BUILD)
endfunction()

macro (create_nodejs_addon)
    if(CAN_BUILD)
		set(cet_file_list "")
  		set(create_nodejs_addon_usage "USAGE: create_nodejs_addon( [ADDON_NAME <addon name>] [LIBRARIES <library list>] [INCLUDES <include directories>])")
  		#message(STATUS "create_nodejs_addon debug: called with ${ARGN} from ${CMAKE_CURRENT_SOURCE_DIR}")
  		cet_parse_args( CNA "ADDON_NAME;LIBRARIES;INCLUDES" "" ${ARGN})
  		# there are no default arguments
  		if( CNA_DEFAULT_ARGS )
     		message(FATAL_ERROR  " undefined arguments ${CNA_DEFAULT_ARGS} \n ${create_nodejs_addon_usage}")
    	endif()

        set (NODE_INCLUDE_DIRS $ENV{NODEJS_INC})

        execute_process(COMMAND node -e "var arr = process.versions.v8.split('.');arr.push('EXTRA');console.log(arr.join(';'));" OUTPUT_VARIABLE V8_STRING)
        list(GET V8_STRING 0 V8_STRING_MAJOR)
        list(GET V8_STRING 1 V8_STRING_MINOR)
        list(GET V8_STRING 2 V8_STRING_PATCH)
        execute_process(COMMAND printf %d%02d%02d ${V8_STRING_MAJOR} ${V8_STRING_MINOR} ${V8_STRING_PATCH} OUTPUT_VARIABLE V8_DEFINE_STRING)
        message("V8_DEFINE_STRING is ${V8_DEFINE_STRING}")

        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unused-result")

        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wno-bad-function-cast -Wno-string-conversion -Wno-deprecated-declarations")
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-bad-function-cast -Wno-string-conversion -Wno-deprecated-declarations")
        elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wno-cast-function-type -Wno-stringop-truncation")
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cast-function-type -Wno-stringop-truncation")
        endif()

        file(GLOB NODEJS_ADDON_SOURCES  *.i)
        file(GLOB LIB_SOURCES  *.cpp)
		file(GLOB SWIG_DEPENDS *.h *.i)

        set_source_files_properties (${NODEJS_ADDON_SOURCES} PROPERTIES CPLUSPLUS ON)
        set_source_files_properties (${NODEJS_ADDON_SOURCES} PROPERTIES SWIG_FLAGS "-node")

				list(APPEND CNA_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${NODE_INCLUDE_DIRS} ${NODE_INCLUDE_DIRS}/node )

	SET(SWIG_MODULE_${CNA_ADDON_NAME}_EXTRA_DEPS ${SWIG_DEPENDS})
        #swig_add_module (${CNA_ADDON_NAME} javascript ${NODEJS_ADDON_SOURCES} ${LIB_SOURCES})
	swig_add_library(${CNA_ADDON_NAME} LANGUAGE javascript SOURCES ${NODEJS_ADDON_SOURCES} ${LIB_SOURCES})

        TARGET_LINK_LIBRARIES (${CNA_ADDON_NAME} ${CNA_LIBRARIES})

        target_include_directories ( ${CNA_ADDON_NAME} PUBLIC ${CNA_INCLUDES})
        #include_directories(${CNA_INCLUDES})
        set_target_properties (${CNA_ADDON_NAME} PROPERTIES
        COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DBUILDING_NODE_EXTENSION -DSWIG_V8_VERSION=0x0${V8_DEFINE_STRING}"
        PREFIX ""
        SUFFIX ".node"
        )

        create_node_package_json(${CNA_ADDON_NAME})

        install (FILES ${TOP_CMAKE_BINARY_DIR}/lib/${CNA_ADDON_NAME}.node DESTINATION ${flavorqual_dir}/lib/node_modules/${CNA_ADDON_NAME})

        add_custom_command(TARGET ${CNA_ADDON_NAME} POST_BUILD 
            COMMAND echo "**** Exports for ${TOP_CMAKE_BINARY_DIR}/lib/${CNA_ADDON_NAME}.node"
        COMMAND echo "**** BEGIN"
        COMMAND /usr/bin/nm ${TOP_CMAKE_BINARY_DIR}/lib/${CNA_ADDON_NAME}.node | /bin/egrep -e \"^[a-f0-9]{1,16} [T]\" | /usr/bin/c++filt  
        COMMAND echo "**** END" )

    else(CAN_BUILD)
        message("Compatible versions of Swig or Node.js not found. NOT building ${CNA_ADDON_NAME}")
    endif(CAN_BUILD)
endmacro()
