cmake_minimum_required (VERSION 3.2)

string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC)
set(MYCMAKE_BUILD_TYPE "Release")

if(BTYPE_UC STREQUAL DEBUG)
    set(MYCMAKE_BUILD_TYPE "Debug")
endif()

if ( NOT EXISTS ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64 )
    include(ExternalProject)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    SET(MYCMAKE_C_FLAGS   "${CMAKE_C_FLAGS_${BTYPE_UC}}   -Wno-uninitialized -Wno-unused-variable")
    SET(MYCMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_${BTYPE_UC}} -Wno-uninitialized -Wno-unused-variable")
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    SET(MYCMAKE_C_FLAGS   "${CMAKE_C_FLAGS_${BTYPE_UC}}   -Wno-maybe-uninitialized -Wno-unused-variable")
    SET(MYCMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_${BTYPE_UC}} -Wno-maybe-uninitialized -Wno-unused-variable")
    endif()

    if( full_qualifier MATCHES "e15")
    set(MYCMAKE_CXX_STANDARD "14")
    else()
    set(MYCMAKE_CXX_STANDARD "17")
    endif()

    ExternalProject_Add(
    mongo-cxx-driver
    GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver
    GIT_TAG r3.6.6
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
    LOG_UPDATE ON
    DEPENDS mongo-c-driver

    #PATCH_COMMAND cd ${TOP_CMAKE_BINARY_DIR}/mongo-cxx-driver-prefix/src/mongo-cxx-driver && git apply ${TOP_CMAKE_SOURCE_DIR}/built-in/mongocxx-r366.patch
    UPDATE_COMMAND ""
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${MYCMAKE_BUILD_TYPE}
        -DCMAKE_BUILD_PARALLEL_LEVEL=${CMAKE_BUILD_PARALLEL_LEVEL}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} 
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} 
        -DMONGOCXX_ENABLE_SSL=ON
        -DBUILD_TESTING=ON -DBUILD_SHARED_LIBS=ON
        -DCMAKE_CXX_FLAGS=${MYCMAKE_CXX_FLAGS}
        -DCMAKE_C_FLAGS=${MYCMAKE_C_FLAGS}
        -DCMAKE_CXX_STANDARD=${MYCMAKE_CXX_STANDARD}
        -DCMAKE_PREFIX_PATH=${TOP_CMAKE_BINARY_DIR}/built-in/mongo-c-driver/lib64/cmake
        -DCMAKE_INSTALL_PREFIX:PATH=${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver
    )
  else()
    add_custom_target( mongo-cxx-driver )
  endif()

      
  if (NOT TARGET mongo::bsoncxx_shared)
    add_library(mongo::bsoncxx_shared SHARED IMPORTED)
    file(MAKE_DIRECTORY ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/include/bsoncxx/v_noabi)
    set_target_properties(mongo::bsoncxx_shared PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/include/bsoncxx/v_noabi"
      IMPORTED_LOCATION "${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/libbsoncxx.so"
      )
      add_dependencies(mongo::bsoncxx_shared mongo-cxx-driver)
  endif()
  if (NOT TARGET mongo::mongocxx_shared)
    add_library(mongo::mongocxx_shared SHARED IMPORTED)
    file(MAKE_DIRECTORY ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/include/mongocxx/v_noabi)
    set_target_properties(mongo::mongocxx_shared PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/include/mongocxx/v_noabi"
      IMPORTED_LOCATION "${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/libmongocxx.so"
      )
      add_dependencies(mongo::mongocxx_shared MONGOCXX::bsoncxx)
  endif()


install(DIRECTORY ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/include/ DESTINATION ${flavorqual_dir}/include )
install(DIRECTORY ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/ DESTINATION ${flavorqual_dir}/lib64/ FILES_MATCHING PATTERN "*.so*" PATTERN "cmake" EXCLUDE PATTERN "pkgconfig" EXCLUDE)
install(DIRECTORY ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/ DESTINATION ${flavorqual_dir}/lib64/ FILES_MATCHING PATTERN "*.a" PATTERN "cmake" EXCLUDE PATTERN "pkgconfig" EXCLUDE)
set(TARGET_FILES 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/cmake/bsoncxx-3.6.6/bsoncxx_targets-$<LOWER_CASE:${MYCMAKE_BUILD_TYPE}>.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/cmake/mongocxx-3.6.6/mongocxx_targets-$<LOWER_CASE:${MYCMAKE_BUILD_TYPE}>.cmake 
)
install(FILES ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/cmake/bsoncxx-3.6.6/bsoncxx_targets.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/cmake/bsoncxx-3.6.6/bsoncxx-config.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/cmake/bsoncxx-3.6.6/bsoncxx-config-version.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/cmake/libbsoncxx-3.6.6/libbsoncxx-config.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/cmake/libbsoncxx-3.6.6/libbsoncxx-config-version.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/cmake/libmongocxx-3.6.6/libmongocxx-config.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/cmake/libmongocxx-3.6.6/libmongocxx-config-version.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/cmake/mongocxx-3.6.6/mongocxx_targets.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/cmake/mongocxx-3.6.6/mongocxx-config.cmake 
              ${TOP_CMAKE_BINARY_DIR}/built-in/mongo-cxx-driver/lib64/cmake/mongocxx-3.6.6/mongocxx-config-version.cmake 
              ${TARGET_FILES}
        DESTINATION ${flavorqual_dir}/lib/artdaq_database/cmake/)
