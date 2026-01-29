IF (FORMAT_WITH_CLANG)
  string(TOUPPER ${FORMAT_WITH_CLANG} FORMAT_WITH_CLANG_UC )
  if( ${FORMAT_WITH_CLANG_UC} MATCHES "TRUE" )
    include(clang-format)
  endif()
ENDIF()

set(ARTDAQ_DATABASE_URI_FILESYSTEM "filesystemdb:///${CMAKE_BINARY_DIR}/databases/filesystemdb/test_db")
set(ARTDAQ_DATABASE_URI_MONGO "mongodb://127.0.0.1:27017/test_db")

set(ARTDAQ_DATABASE_URI_UCOND $ENV{ARTDAQ_DATABASE_URI_UCOND})
set(ARTDAQ_DATABASE_AUTH $ENV{ARTDAQ_DATABASE_AUTH})

find_program(mongod_bin name mongod HINTS $ENV{HOME} $ENV{MONGODB_FQ_DIR})

configure_file("CTestCustom.cmake.in" ${CMAKE_BINARY_DIR}/CTestCustom.cmake)
configure_file("ctest_pre_test.sh.in" ${CMAKE_BINARY_DIR}/ctest_pre_test.sh @ONLY FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
configure_file("ctest_post_test.sh.in" ${CMAKE_BINARY_DIR}/ctest_post_test.sh @ONLY FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)

#test order is important
add_subdirectory(JsonDocument)
add_subdirectory(Overlay)
add_subdirectory(DataFormats)
add_subdirectory(StorageProviders)

add_subdirectory(ConfigurationDB)
add_subdirectory(CaseStudy)
