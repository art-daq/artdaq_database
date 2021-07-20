include(CetTest)
cet_enable_asserts()

link_libraries(
  artdaq-database_OtsDaqCPPInterface
)


cet_make_exec(DatabaseConfiguration_t
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
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}	
) 

MESSAGE("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}f")
cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}f")
cet_test(OtsDaqCPPInterface_filesystem_t USE_BOOST_UNIT HANDBUILT TEST_EXEC DatabaseConfiguration_t)

MESSAGE("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}f")
cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}f")
cet_test(OtsDaqCPPInterface_mongo_t USE_BOOST_UNIT HANDBUILT TEST_EXEC DatabaseConfiguration_t)

install_headers()
install_source()

