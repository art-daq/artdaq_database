add_executable(FileSystemFhicl_t FileSystemFhicl_t.cc)

target_link_libraries(FileSystemFhicl_t PRIVATE
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders_FileSystemDB
  Boost::program_options
)

add_subdirectory(test001)
##add_subdirectory(test002) #not included into the search index
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
