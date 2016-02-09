find_ups_product( mongodb v3_0_7 )

include_directories($ENV{MONGODB_FQ_DIR}/include/bsoncxx/v0.3)

find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_FQ_DIR}/lib )

art_make_exec(JSONDocument_t
  LIBRARIES
  artdaq-database_StackTrace
  artdaq-database_JsonDocument
  ${BSONCXXLIB}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

#replace tests
add_subdirectory(test001)
add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)

#insert tests
add_subdirectory(test101)
add_subdirectory(test102)
add_subdirectory(test103)
add_subdirectory(test104)

#delete tests
add_subdirectory(test201)

#collection tests
add_subdirectory(test301)
add_subdirectory(test302)

install_source()
