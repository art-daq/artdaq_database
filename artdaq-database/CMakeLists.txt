include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(BuildInfo)
add_subdirectory(SharedCommon)

add_subdirectory(DataFormats)
add_subdirectory(Overlay)
add_subdirectory(JsonDocument)

add_subdirectory(BasicTypes)
add_subdirectory(DemoTypes)

add_subdirectory(StorageProviders)
add_subdirectory(ConfigurationDB)
#add_subdirectory(JsonConvert)

#add_subdirectory(Utilities)
#add_subdirectory(SWIGBindings)




