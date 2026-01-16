add_executable(MongoFhicl_t MongoFhicl_t.cc)

target_link_libraries(MongoFhicl_t PRIVATE
  artdaq-database::SharedCommon
  artdaq-database::BasicTypes
  artdaq-database::DataFormats_Fhicl
  artdaq-database::DataFormats_Json
  artdaq-database::JsonDocument
  artdaq-database::StorageProviders_MongoDB
  Boost::program_options
)

add_subdirectory(test001)
add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
