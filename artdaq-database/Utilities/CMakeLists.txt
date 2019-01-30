cet_make_exec(conftool
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
) 

cet_make_exec(readjson
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Fhicl
  artdaq-database_JsonDocument

  ${Boost_PROGRAM_OPTIONS_LIBRARY}
)


cet_make_exec(readfhicl
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Fhicl
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
cet_make_exec(refactorfhicl
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Fhicl
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
)
endif()

cet_make_exec(fhicl2json
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_JsonDocument
  artdaq-database_DataFormats_Fhicl
  ${FHICLCPP}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

cet_make_exec(bulkloader
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_BasicTypes
  
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  ${FHICLCPP}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  -lpthread
)

cet_make_exec(bulkdownloader
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_BasicTypes

  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  ${FHICLCPP}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  -lpthread
)


cet_make_exec(migrate_database
  LIBRARIES
    
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB  
  ${Boost_PROGRAM_OPTIONS_LIBRARY} 
)

cet_make_exec(rebuild_database_index
  LIBRARIES
    
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB  
  ${Boost_PROGRAM_OPTIONS_LIBRARY} 
)

cet_make_exec(fixtestjson
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Fhicl
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
)


install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/schema.fcl DESTINATION ${flavorqual_dir}/conf/)

cet_script(conftool.py)

install_headers()
install_source()
