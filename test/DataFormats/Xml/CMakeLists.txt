add_executable(XMLConvert_t XMLConvert_t.cc)

target_link_libraries(XMLConvert_t PRIVATE
  artdaq-database_DataFormats_Json
  artdaq-database_DataFormats_Xml
  Boost::program_options
)

add_subdirectory(test001)
add_subdirectory(test002)

install_source()
