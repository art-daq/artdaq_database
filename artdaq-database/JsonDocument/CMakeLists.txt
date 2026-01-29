cet_make_library(
  SOURCE
    JSONDocumentBuilder.cpp
    JSONDocument.cpp
    JSONDocumentMigrator.cpp
    JSONDocument_utils.cpp
  LIBRARIES
    artdaq-database::BasicTypes
    artdaq-database::DataFormats_Json
    artdaq-database::Overlay
  )

install_headers()
install_source()
