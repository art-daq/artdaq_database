cet_make_library(
	SOURCE
configuration_dbproviders.cpp
dboperation_exportimport.cpp
dboperation_managealiases.cpp
dboperation_manageconfigs.cpp
dboperation_managedocument.cpp
dboperation_metadata.cpp
dboperation_searchcollection.cpp
detail_exportimport.cpp
detail_managealiases.cpp
detail_manageconfigs.cpp
detail_managedocument.cpp
detail_metadata.cpp
detail_searchcollection.cpp
dispatch_filedb.cpp
dispatch_mongodb.cpp
dispatch_signatures.cpp
dispatch_ucondb.cpp
enable_trace.cpp
Multitasker.cpp
options_operation_base.cpp
options_operation_bulkoperations.cpp
options_operation_managealiases.cpp
options_operation_manageconfigs.cpp
options_operation_managedocument.cpp
shared_helper_functions.cpp
  LIBRARIES
  ${FHICLCPP_DATAFORMAT_LIB}
  artdaq-database_DataFormats_Json
  artdaq-database_DataFormats_Xml  
  artdaq-database_JsonDocument
  artdaq-database_BasicTypes
  artdaq-database_StorageProviders_MongoDB  
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  Boost::program_options
  cetlib_except
  -lpthread
  )

  if(USE_FHICLCPP)
  target_compile_definitions(artdaq-database_ConfigurationDB PUBLIC USE_FHICLCPP)
  endif()

install_headers()
install_source()
