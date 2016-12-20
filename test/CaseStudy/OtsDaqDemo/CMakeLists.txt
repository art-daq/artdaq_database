cet_make_exec(OtsDaqDemo_t
  LIBRARIES
  artdaq-database_StackTrace  
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_common
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UcondDB  
  ${FHICLCPP}
  ${SQLITE3}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
) 

SET (TEST_COLLECTION_NAME "testLoadStore" )
string(RANDOM LENGTH 5 ALPHABET 0123456789 VERSION_NUMBER)
message(STATUS "VERSION_NUMBER=${VERSION_NUMBER}")
SET (TEST_VERSION_NAME    versionA${VERSION_NUMBER} )

#add_subdirectory(test001)
#add_subdirectory(test002)
add_subdirectory(test003)
#add_subdirectory(test004)
#add_subdirectory(test005)
install_source()
