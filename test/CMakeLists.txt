cet_enable_asserts()

IF (FORMAT_WITH_CLANG)
  string(TOUPPER ${FORMAT_WITH_CLANG} FORMAT_WITH_CLANG_UC )
  if( ${FORMAT_WITH_CLANG_UC} MATCHES "TRUE" )
    include(clang-format)
  endif()
ENDIF()

# Make sure tests have correct environment settings.
include(CetTest)

set(ARTDAQ_DATABASE_URI_FILESYSTEM "filesystemdb://${CMAKE_BINARY_DIR}/databases/filesystemdb/test_db")
set(ARTDAQ_DATABASE_URI_MONGO "mongodb://127.0.0.1:27017/test_db")

set(ARTDAQ_DATABASE_URI_UCOND $ENV{ARTDAQ_DATABASE_URI_UCOND})
set(ARTDAQ_DATABASE_AUTH $ENV{ARTDAQ_DATABASE_AUTH})

cet_test_env("FHICL_FILE_PATH=.")

cet_test_env("PATH=${TOP_CMAKE_BINARY_DIR}/bin:$ENV{PATH}")
cet_test_env("LD_LIBRARY_PATH=${TOP_CMAKE_BINARY_DIR}/lib:$ENV{LD_LIBRARY_PATH}")

find_program(mongod_bin name mongod HINTS $ENV{HOME} $ENV{MONGODB_FQ_DIR})

configure_file("CTestCustom.cmake.in" ${CMAKE_BINARY_DIR}/CTestCustom.cmake)

#test order is important
#add_subdirectory(JsonDocument)
#add_subdirectory(Overlay)
#add_subdirectory(DataFormats)
add_subdirectory(StorageProviders)

add_subdirectory(ConfigurationDB)
add_subdirectory(CaseStudy)

