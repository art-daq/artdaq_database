include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${TOP_CMAKE_SOURCE_DIR}
 ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp
 ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib
 $ENV{MONGODB_INC}) # this is really bad -- we don't know why find_ups_product isn't doing this

IF (FORMAT_WITH_CLANG)
  string(TOUPPER ${FORMAT_WITH_CLANG} FORMAT_WITH_CLANG_UC )
  if( ${FORMAT_WITH_CLANG_UC} MATCHES "TRUE" )
    include(clang-format)
  endif()
ENDIF()

message(STATUS "FHICLCPP='${FHICLCPP}'")

install(DIRECTORY deployment-scripts DESTINATION ${product}/${version})
cet_script(deployment-scripts/install_artdaq_database.sh)

add_subdirectory(BuildInfo)
add_subdirectory(SharedCommon)

add_subdirectory(DataFormats)
add_subdirectory(Overlay)
add_subdirectory(JsonDocument)

add_subdirectory(BasicTypes)

add_subdirectory(StorageProviders)
add_subdirectory(ConfigurationDB)
#add_subdirectory(JsonConvert)

add_subdirectory(Utilities)
add_subdirectory(SWIGBindings)




