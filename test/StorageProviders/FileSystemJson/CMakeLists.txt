add_executable(FileSystemJson_t FileSystemJson_t.cc)

target_link_libraries(FileSystemJson_t PRIVATE
  artdaq-database::SharedCommon
  artdaq-database::BasicTypes
  artdaq-database::DataFormats_Json
  artdaq-database::JsonDocument
  artdaq-database::StorageProviders_FileSystemDB
  Boost::program_options
)

add_subdirectory(test001)
add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
