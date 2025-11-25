cet_make_exec(NAME ValidateJson_t
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
  Boost::program_options
)

SET ( TEST_NAME "ValidateJson" )

# Register test with CTest
add_test(NAME ${TEST_NAME}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ValidateJson_t 
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
