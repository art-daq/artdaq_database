add_executable(FileSystemXml_t FileSystemXml_t.cc)

target_link_libraries(FileSystemXml_t PRIVATE
  artdaq-database::BasicTypes
  artdaq-database::DataFormats_Json
  artdaq-database::DataFormats_Xml
  artdaq-database::JsonDocument
  artdaq-database::StorageProviders_FileSystemDB
  Boost::program_options
)

add_subdirectory(test001)
#add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
