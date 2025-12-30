add_executable(ConvertJson_t ConvertJson_t.cc)

target_link_libraries(ConvertJson_t PRIVATE
  artdaq-database_DataFormats_Json
  Boost::program_options
)

add_subdirectory(test001)
add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
