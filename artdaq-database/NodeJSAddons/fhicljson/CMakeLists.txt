set(NODEJS_ADDON_NAME "fhicljson" )

art_make(
  LIB_LIBRARIES
  artdaq-database_FhiclJson
  ${FHICLCPP}
  ${CETLIB}
  )
  
set (NODEJS_ADDON_INCLUDES ${CMAKE_SOURCE_DIR}/artdaq-database)

set (NODEJS_ADDON_LIBS "artdaq-database_FhiclJson")

create_nodejs_addon ( ${NODEJS_ADDON_NAME} ${NODEJS_ADDON_INCLUDES} ${NODEJS_ADDON_LIBS})

