set(NODEJS_ADDON_NAME "conftoolg" )

art_make(
  LIB_LIBRARIES
  artdaq-database_ConfigurationDB
  ${FHICLCPP}
  ${CETLIB}
  )

set (NODEJS_ADDON_INCLUDES ${CMAKE_SOURCE_DIR}/artdaq-database)

set (NODEJS_ADDON_LIBS "artdaq-database_ConfigurationDB")

create_nodejs_addon ( ${NODEJS_ADDON_NAME} ${NODEJS_ADDON_INCLUDES} ${NODEJS_ADDON_LIBS})

