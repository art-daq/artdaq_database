cmake_minimum_required (VERSION 3.2)

if ( NOT EXISTS ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64 )
    include(ExternalProject)
    string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC)
    set(MYCMAKE_BUILD_TYPE "Release")

    if(BTYPE_UC STREQUAL DEBUG)
    set(MYCMAKE_BUILD_TYPE "Debug")
    endif()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      SET(MYCMAKE_C_FLAGS   "${CMAKE_C_FLAGS_${BTYPE_UC}}   -Wno-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-missing-field-initializers -Wno-missing-braces")
      SET(MYCMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_${BTYPE_UC}} -Wno-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-missing-field-initializers -Wno-missing-braces")
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
      SET(MYCMAKE_C_FLAGS   "${CMAKE_C_FLAGS_${BTYPE_UC}}   -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-missing-field-initializers -Wno-old-style-declaration")
      SET(MYCMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_${BTYPE_UC}} -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-missing-field-initializers -Wno-old-style-declaration")
    endif()

    if( full_qualifier MATCHES "e15")
    set(MYCMAKE_CXX_STANDARD "14")
    else()
    set(MYCMAKE_CXX_STANDARD "17")
    endif()

    ExternalProject_Add(
    mongo-c-driver
    GIT_REPOSITORY https://github.com/mongodb/mongo-c-driver
    GIT_TAG 1.19.2
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
    LOG_UPDATE ON

    PATCH_COMMAND	 ""
    UPDATE_COMMAND ""
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${MYCMAKE_BUILD_TYPE}
        -DCMAKE_BUILD_PARALLEL_LEVEL=${CMAKE_BUILD_PARALLEL_LEVEL}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} 
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
				-DMONGOC_ENABLE_SSL=ON
				-DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=ON -DENABLE_SASL=OFF -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DENABLE_SHM_COUNTERS=OFF 
        -DCMAKE_CXX_FLAGS=${MYCMAKE_CXX_FLAGS}
        -DCMAKE_C_FLAGS=${MYCMAKE_C_FLAGS}
        -DCMAKE_CXX_STANDARD=${MYCMAKE_CXX_STANDARD}
        -DCMAKE_INSTALL_PREFIX:PATH=${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver 
    )
else()
    add_custom_target( mongo-c-driver )
endif()

include_directories(${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/include/libbson-1.0)
include_directories(${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/include/libmongoc-1.0)
link_directories(${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64)

install(DIRECTORY ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/include/libbson-1.0/ DESTINATION ${product}/${version}/include )
install(DIRECTORY ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/include/libmongoc-1.0/ DESTINATION ${product}/${version}/include )
install(DIRECTORY ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/ DESTINATION ${flavorqual_dir}/lib )
