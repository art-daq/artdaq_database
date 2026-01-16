add_executable(JSONDocumentBuilder_t JSONDocumentBuilder_t.cc)

target_link_libraries(JSONDocumentBuilder_t PRIVATE
  artdaq-database::JsonDocument
  Boost::program_options
)

#build document tests
add_subdirectory(test001)
add_subdirectory(test002)
add_subdirectory(test003)

#alias add remove tests
add_subdirectory(test004)
add_subdirectory(test005)

#add to global configuration
add_subdirectory(test006)

#set version
add_subdirectory(test007)

#mark readonly and deleted
add_subdirectory(test008)
add_subdirectory(test009)

#entity add remove tests
add_subdirectory(test010)
add_subdirectory(test011)

install_source()
