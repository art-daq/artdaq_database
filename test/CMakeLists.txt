cet_enable_asserts()

configure_file("CTestCustom.cmake.in" ${CMAKE_BINARY_DIR}/CTestCustom.cmake)

# Make sure tests have correct environment settings.
include(CetTest)

set(ARTDAQ_DATABASE_URI_FILESYSTEM "filesystemdb://${CMAKE_BINARY_DIR}/databases/filesystemdb/test_db")
set(ARTDAQ_DATABASE_URI_MONGO "mongodb://127.0.0.1:27017/test_db")

cet_test_env("FHICL_FILE_PATH=.")

#test order is important
#add_subdirectory(JsonDocument)
#add_subdirectory(DataFormats)
add_subdirectory(ConfigurationDB)
#add_subdirectory(CaseStudy)
#add_subdirectory(StorageProviders)

#add_subdirectory(JSONConvert)
