cet_make_exec(NAME OtsDaqDemo_t
  LIBRARIES
    
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB  
  fhiclcpp
  #${SQLITE3}
  cetlib
  cetlib_except
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
