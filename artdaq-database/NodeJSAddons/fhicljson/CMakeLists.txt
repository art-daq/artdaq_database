set(NODEJS_ADDON_NAME "fhicljson" )

#project(artdaq-database_${NODEJS_ADDON_NAME})

set (NODEJS_ADDON_INCLUDES ${CMAKE_SOURCE_DIR}/artdaq-database)

set (NODEJS_ADDON_LIBS "artdaq-database_BuildInfo")

create_nodejs_addon ( ${NODEJS_ADDON_NAME} ${NODEJS_ADDON_INCLUDES} ${NODEJS_ADDON_LIBS})

