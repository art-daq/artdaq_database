add_executable(CONFConvert_t CONFConvert_t.cc)

target_link_libraries(CONFConvert_t PRIVATE
  artdaq-database::DataFormats_Json
  artdaq-database::DataFormats_Conf
)

add_subdirectory(test001)
add_subdirectory(test002)

install_source()
