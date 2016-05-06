cet_enable_asserts()

# Make sure tests have correct environment settings.
include(CetTest)
# If you explicitly include CetTest in a subdirectory, you will need to
# re-initialize the test environment.
cet_test_env("FHICL_FILE_PATH=.")

#test order is important
#add_subdirectory(JsonDocument)
add_subdirectory(FhiclJson)
#add_subdirectory(XmlJson)
#add_subdirectory(ConfigurationDB)
#add_subdirectory(StorageProviders)
#add_subdirectory(JSONConvert)
