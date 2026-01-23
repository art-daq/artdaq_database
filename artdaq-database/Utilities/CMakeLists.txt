cet_make_exec(NAME conftool
  LIBRARIES
  artdaq-database::SharedCommon
  artdaq-database::BasicTypes
  artdaq-database::DataFormats_Json
  artdaq-database::JsonDocument
  artdaq-database::ConfigurationDB
  artdaq-database::StorageProviders_MongoDB
  artdaq-database::StorageProviders_FileSystemDB
  artdaq-database::StorageProviders_UconDB
  Boost::program_options
)

cet_make_exec(NAME readjson
  LIBRARIES
  artdaq-database::SharedCommon
  artdaq-database::DataFormats_Json
  artdaq-database::JsonDocument

  Boost::program_options
)

cet_make_exec(NAME readfhicl
  LIBRARIES
  artdaq-database::SharedCommon
  artdaq-database::DataFormats_Fhicl
  Boost::program_options
)

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
cet_make_exec(NAME refactorfhicl
  LIBRARIES
  artdaq-database::SharedCommon
  artdaq-database::DataFormats_Fhicl
  Boost::program_options
)
endif()

cet_make_exec(NAME fhicl2json
  LIBRARIES
  artdaq-database::SharedCommon
  artdaq-database::JsonDocument
  artdaq-database::DataFormats_Fhicl
  Boost::program_options
)

cet_make_exec(NAME bulkloader
  LIBRARIES
  artdaq-database::SharedCommon
  artdaq-database::BasicTypes

  artdaq-database::DataFormats_Json
  artdaq-database::JsonDocument
  artdaq-database::ConfigurationDB
  artdaq-database::StorageProviders_MongoDB
  artdaq-database::StorageProviders_FileSystemDB
  artdaq-database::StorageProviders_UconDB
  Boost::program_options
  -lpthread
)

cet_make_exec(NAME bulkdownloader
  LIBRARIES
  artdaq-database::SharedCommon
  artdaq-database::BasicTypes

  artdaq-database::DataFormats_Json
  artdaq-database::JsonDocument
  artdaq-database::ConfigurationDB
  artdaq-database::StorageProviders_MongoDB
  artdaq-database::StorageProviders_FileSystemDB
  artdaq-database::StorageProviders_UconDB
  Boost::program_options
  -lpthread
)


cet_make_exec(NAME migrate_database
  LIBRARIES

  artdaq-database::BasicTypes
  artdaq-database::DataFormats_Json
  artdaq-database::JsonDocument
  artdaq-database::ConfigurationDB
  artdaq-database::StorageProviders_MongoDB
  artdaq-database::StorageProviders_FileSystemDB
  artdaq-database::StorageProviders_UconDB
  Boost::program_options
)

cet_make_exec(NAME rebuild_database_index
  LIBRARIES

  artdaq-database::BasicTypes
  artdaq-database::DataFormats_Json
  artdaq-database::JsonDocument
  artdaq-database::ConfigurationDB
  artdaq-database::StorageProviders_MongoDB
  artdaq-database::StorageProviders_FileSystemDB
  artdaq-database::StorageProviders_UconDB
  Boost::program_options
)

cet_make_exec(NAME fixtestjson
  LIBRARIES
  artdaq-database::SharedCommon
  artdaq-database::DataFormats_Json
  Boost::program_options
)


install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/schema.fcl DESTINATION ./${flavorqual_dir}/conf/)

cet_script(conftool.py)

install_headers()
install_source()
