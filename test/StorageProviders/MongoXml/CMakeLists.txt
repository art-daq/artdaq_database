add_executable(MongoXml_t MongoXml_t.cc)

target_link_libraries(MongoXml_t PRIVATE
  artdaq-database::BasicTypes
  artdaq-database::DataFormats_Json
  artdaq-database::DataFormats_Xml
  artdaq-database::JsonDocument
  artdaq-database::StorageProviders_MongoDB
  Boost::program_options
)

add_subdirectory(test001)
add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
