include(ExternalProject)

ExternalProject_Add(
  mongo-cxx-driver
  GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver
  GIT_TAG r3.4.0
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON
  LOG_INSTALL ON
  LOG_UPDATE ON
  UPDATE_COMMAND ""
  CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} 
   -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} 
   -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
   -DBUILD_TESTING=Off -DBUILD_SHARED_LIBS=On
   -DCMAKE_INSTALL_PREFIX:PATH=${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver 
)

include_directories(${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/include)
include_directories(${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/include/bsoncxx/v_noabi)
include_directories(${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/include/mongocxx/v_noabi)
link_directories(${TOP_CMAKE_BINARY_DIR}/mongo-cxx-driver-prefix/src/mongo-cxx-driver-build/src/bsoncxx)
link_directories(${TOP_CMAKE_BINARY_DIR}/mongo-cxx-driver-prefix/src/mongo-cxx-driver-build/src/mongocxx)


install(DIRECTORY ${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/include/bsoncxx/v_noabi/bsoncxx DESTINATION ${product}/${version}/include )
install(DIRECTORY ${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/include/mongocxx/v_noabi/mongocxx DESTINATION ${product}/${version}/include )
install(DIRECTORY ${TOP_CMAKE_SOURCE_DIR}/built-in/mongo-cxx-driver/lib64/ DESTINATION ${flavorqual_dir}/lib )
