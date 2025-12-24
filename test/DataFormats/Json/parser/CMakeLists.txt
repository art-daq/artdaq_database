add_executable(readjson_t readjson_t.cc)

target_link_libraries(readjson_t PRIVATE
  artdaq-database_DataFormats_Json
  Boost::program_options
)

add_subdirectory(test001)

install_source()
