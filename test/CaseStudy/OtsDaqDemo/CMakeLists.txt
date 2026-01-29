add_executable(OtsDaqDemo_t OtsDaqDemo_t.cc)

target_link_libraries(OtsDaqDemo_t PRIVATE
  artdaq-database::BasicTypes
  artdaq-database::DataFormats_Fhicl
  artdaq-database::DataFormats_Json
  artdaq-database::JsonDocument
  artdaq-database::ConfigurationDB
  artdaq-database::StorageProviders_MongoDB
  artdaq-database::StorageProviders_FileSystemDB
  artdaq-database::StorageProviders_UconDB
  Boost::program_options
)

SET (TEST_COLLECTION_NAME "testReadWrite" )
string(RANDOM LENGTH 5 ALPHABET 0123456789 VERSION_NUMBER)
message(STATUS "VERSION_NUMBER=${VERSION_NUMBER}")
SET (TEST_VERSION_NAME    versionA${VERSION_NUMBER} )

set (ARTDAQ_DATABASE_URI_OTS_FILESYSTEM ${ARTDAQ_DATABASE_URI_FILESYSTEM}ots)
set (ARTDAQ_DATABASE_URI_OTS_MONGO ${ARTDAQ_DATABASE_URI_MONGO}ots)

add_subdirectory(test003)
#add_subdirectory(test004)
#add_subdirectory(test005)

install_source()
