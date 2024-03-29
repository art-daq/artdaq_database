cmake_minimum_required (VERSION 3.20)

 string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC)
 set(MYCMAKE_BUILD_TYPE "Release")

 if(BTYPE_UC STREQUAL DEBUG)
    set(MYCMAKE_BUILD_TYPE "Debug")
 endif()

if ( NOT EXISTS ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64 )
    include(ExternalProject)

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
    GIT_TAG 1.24.4
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
    LOG_UPDATE ON
    
    PATCH_COMMAND cd ${TOP_CMAKE_BINARY_DIR}/mongo-c-driver-prefix/src/mongo-c-driver && git apply ${TOP_CMAKE_SOURCE_DIR}/built-in/mongoc-1.24.4.patch
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

install(DIRECTORY ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/include/ DESTINATION ./${flavorqual_dir}/include )
install(DIRECTORY ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/ DESTINATION ./${flavorqual_dir}/lib64/ FILES_MATCHING PATTERN "*.so*" PATTERN "cmake" EXCLUDE)
install(DIRECTORY ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/ DESTINATION ./${flavorqual_dir}/lib64/ FILES_MATCHING PATTERN "*.a" PATTERN "cmake" EXCLUDE)
set(TARGET_FILES 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/bson-1.0/bson-targets-$<LOWER_CASE:${MYCMAKE_BUILD_TYPE}>.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/mongoc-1.0/mongoc-targets-$<LOWER_CASE:${MYCMAKE_BUILD_TYPE}>.cmake 
)
install(FILES ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/bson-1.0/bson-targets.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/bson-1.0/bson-1.0-config.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/bson-1.0/bson-1.0-config-version.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/libbson-1.0/libbson-1.0-config.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/libbson-1.0/libbson-1.0-config-version.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/libbson-static-1.0/libbson-static-1.0-config.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/libbson-static-1.0/libbson-static-1.0-config-version.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/libmongoc-1.0/libmongoc-1.0-config.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/libmongoc-1.0/libmongoc-1.0-config-version.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/libmongoc-static-1.0/libmongoc-static-1.0-config.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/libmongoc-static-1.0/libmongoc-static-1.0-config-version.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/mongoc-1.0/mongoc-targets.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/mongoc-1.0/mongoc-1.0-config.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake/mongoc-1.0/mongoc-1.0-config-version.cmake 
              ${TARGET_FILES}
        DESTINATION ./${flavorqual_dir}/lib/artdaq_database/cmake/)
