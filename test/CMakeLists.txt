cet_enable_asserts()

configure_file("CTestCustom.cmake.in" ${CMAKE_BINARY_DIR}/CTestCustom.cmake)

# Make sure tests have correct environment settings.
include(CetTest)

cet_test_env("FHICL_FILE_PATH=.")
cet_test_env("ARTDAQ_DATABASE_DATADIR=${CMAKE_BINARY_DIR}/databases")

#test order is important
add_subdirectory(JsonDocument)
add_subdirectory(DataFormats)
add_subdirectory(StorageProviders)
#add_subdirectory(ConfigurationDB)
#add_subdirectory(CaseStudy)

#add_subdirectory(JSONConvert)


