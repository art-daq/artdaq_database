add_executable(ConfToolTest_t ConfToolTest_t.cc)

target_link_libraries(ConfToolTest_t PRIVATE
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
#add_subdirectory(test002)

#add_subdirectory(test003)
#add_subdirectory(test004)
#add_subdirectory(test005)

#add_subdirectory(test007)
#add_subdirectory(test008)

install_source()
