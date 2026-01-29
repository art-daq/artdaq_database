add_executable(ConfigurationDB_t ConfigurationDB_t.cc)

target_link_libraries(ConfigurationDB_t PRIVATE
  artdaq-database::BasicTypes
  artdaq-database::DataFormats_Fhicl
  artdaq-database::DataFormats_Json
  artdaq-database::JsonDocument
  artdaq-database::ConfigurationDB
  artdaq-database::StorageProviders_MongoDB
  artdaq-database::StorageProviders_FileSystemDB
  artdaq-database::StorageProviders_UconDB
)


add_subdirectory(ReadWrite)
add_subdirectory(UtilsOperations)
add_subdirectory(ManageConfigs)
add_subdirectory(BulkOperations)
add_subdirectory(ManageDocuments)
add_subdirectory(ManageAliases)


add_subdirectory(ExportImport)
add_subdirectory(MigrateDatabase)

add_subdirectory(Utils)
