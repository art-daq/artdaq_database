add_executable(JSONConvert_t JSONConvert_t.cc)

target_link_libraries(JSONConvert_t PRIVATE
  artdaq-database::BasicTypes
  artdaq-database::FhiclJson
  artdaq-database::JsonDocument
  artdaq-database::ConfigurationDB
  artdaq-database::StorageProviders_MongoDB
  artdaq-database::StorageProviders_FileSystemDB
  artdaq-database::StorageProviders_UconDB
  Boost::program_options
)


add_subdirectory(test001)
install_source()
