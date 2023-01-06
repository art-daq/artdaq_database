cet_make_exec(NAME conftool
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  Boost::program_options  
) 

cet_make_exec(NAME readjson
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument

  Boost::program_options
)

if(USE_FHICL)
cet_make_exec(NAME readfhicl
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Fhicl
  fhiclcpp
  cetlib
  cetlib_except
  Boost::program_options
)

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
cet_make_exec(NAME refactorfhicl
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Fhicl
  fhiclcpp
  cetlib
  cetlib_except
  Boost::program_options
)
endif()

cet_make_exec(NAME fhicl2json
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_JsonDocument
  artdaq-database_DataFormats_Fhicl
  fhiclcpp
  cetlib
  cetlib_except
  Boost::program_options
)
endif()

cet_make_exec(NAME bulkloader
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_BasicTypes
  
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  cetlib
  cetlib_except
  Boost::program_options
  -lpthread
)

cet_make_exec(NAME bulkdownloader
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_BasicTypes

  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  cetlib
  cetlib_except
  Boost::program_options
  -lpthread
)


cet_make_exec(NAME migrate_database
  LIBRARIES
    
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB  
  Boost::program_options 
  cetlib_except
)

cet_make_exec(NAME rebuild_database_index
  LIBRARIES
    
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB  
  Boost::program_options 
  cetlib_except
)

cet_make_exec(NAME fixtestjson
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Json
  Boost::program_options
  cetlib_except
)


install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/schema.fcl DESTINATION ./${flavorqual_dir}/conf/)

cet_script(conftool.py)

install_headers()
install_source()
