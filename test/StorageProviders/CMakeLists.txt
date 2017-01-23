cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}a")
add_subdirectory(MongoJson)
add_subdirectory(MongoFhicl)
add_subdirectory(MongoXml)

cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}a")
add_subdirectory(FileSystemJson)
add_subdirectory(FileSystemFhicl)
add_subdirectory(FileSystemXml)

cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_UCOND}")
add_subdirectory(UcondJson)


#add_subdirectory(UcondFhicl)
#add_subdirectory(UcondXml)

install_source()
