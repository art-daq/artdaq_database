cmake_minimum_required (VERSION 3.2)

if ( NOT EXISTS ${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/lib64 )
    include(ExternalProject)
    string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC)
    set(MYCMAKE_BUILD_TYPE "Release")

    if(BTYPE_UC STREQUAL DEBUG)
    set(MYCMAKE_BUILD_TYPE "Debug")
    endif()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    SET(MYCMAKE_C_FLAGS   "${CMAKE_C_FLAGS_${BTYPE_UC}}   -Wno-uninitialized")
    SET(MYCMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_${BTYPE_UC}} -Wno-uninitialized")
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    SET(MYCMAKE_C_FLAGS   "${CMAKE_C_FLAGS_${BTYPE_UC}}   -Wno-maybe-uninitialized")
    SET(MYCMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_${BTYPE_UC}} -Wno-maybe-uninitialized")
    endif()

    if( full_qualifier MATCHES "e15")
    set(MYCMAKE_CXX_STANDARD "14")
    else()
    set(MYCMAKE_CXX_STANDARD "17")
    endif()

    ExternalProject_Add(
    mongo-cxx-driver
    GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver
    GIT_TAG 23f3d1d3b0f2bdbb247683cdcfb0e77b221841ba
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
    LOG_UPDATE ON
    PATCH_COMMAND ${CMAKE_COMMAND} -E chdir ${TOP_CMAKE_BINARY_DIR}/mongo-cxx-driver-prefix/src/mongo-cxx-driver 
        git apply ${TOP_CMAKE_SOURCE_DIR}/built-in/mongocxx.patch
    
    UPDATE_COMMAND ""  
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${MYCMAKE_BUILD_TYPE}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} 
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} 
        -DBUILD_TESTING=Off -DBUILD_SHARED_LIBS=On
        -DCMAKE_CXX_FLAGS=${MYCMAKE_CXX_FLAGS}
        -DCMAKE_C_FLAGS=${MYCMAKE_C_FLAGS}
        -DCMAKE_CXX_STANDARD=${MYCMAKE_CXX_STANDARD}
        -DCMAKE_INSTALL_PREFIX:PATH=${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver 
    )
else()
    add_custom_target(     mongo-cxx-driver )
endif()

include_directories(${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/include)
include_directories(${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/include/bsoncxx/v_noabi)
include_directories(${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/include/mongocxx/v_noabi)
link_directories(${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/lib64)

#link_directories(${TOP_CMAKE_BINARY_DIR}/mongo-cxx-driver-prefix/src/mongo-cxx-driver-build/src/bsoncxx)
#link_directories(${TOP_CMAKE_BINARY_DIR}/mongo-cxx-driver-prefix/src/mongo-cxx-driver-build/src/mongocxx)


install(DIRECTORY ${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/include/bsoncxx/v_noabi/bsoncxx DESTINATION ${product}/${version}/include )
install(DIRECTORY ${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/include/mongocxx/v_noabi/mongocxx DESTINATION ${product}/${version}/include )
install(DIRECTORY ${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/lib64/ DESTINATION ${flavorqual_dir}/lib )
