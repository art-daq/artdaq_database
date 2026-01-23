add_executable(FileSystemFhicl_t FileSystemFhicl_t.cc)

target_link_libraries(FileSystemFhicl_t PRIVATE
  artdaq-database::BasicTypes
  artdaq-database::DataFormats_Fhicl
  artdaq-database::DataFormats_Json
  artdaq-database::JsonDocument
  artdaq-database::StorageProviders_FileSystemDB
  Boost::program_options
)

add_subdirectory(test001)
##add_subdirectory(test002) #not included into the search index
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
