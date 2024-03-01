cet_make_library(
  SOURCE
    JSONDocumentBuilder.cpp
    JSONDocument.cpp
    JSONDocumentMigrator.cpp
    JSONDocument_utils.cpp
  LIBRARIES
    artdaq-database_BasicTypes
    artdaq-database_DataFormats_Json
    artdaq-database_Overlay
    cetlib_except
  )

install_headers()
install_source()
