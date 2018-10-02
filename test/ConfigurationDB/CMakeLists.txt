cet_make_exec(ConfigurationDB_t
  LIBRARIES
    
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB  
) 


add_subdirectory(ReadWrite)
#add_subdirectory(UtilsOperations)
#add_subdirectory(ManageConfigs)
#add_subdirectory(BulkOperations)
#add_subdirectory(ManageDocuments)
#add_subdirectory(ManageAliases)


#add_subdirectory(ExportImport)
#add_subdirectory(MigrateDatabase)

##add_subdirectory(Utils)

