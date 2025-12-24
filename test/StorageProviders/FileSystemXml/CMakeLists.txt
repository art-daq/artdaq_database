add_executable(FileSystemXml_t FileSystemXml_t.cc)

target_link_libraries(FileSystemXml_t PRIVATE
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Json
  artdaq-database_DataFormats_Xml
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders_FileSystemDB
  Boost::program_options
)

add_subdirectory(test001)
#add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
