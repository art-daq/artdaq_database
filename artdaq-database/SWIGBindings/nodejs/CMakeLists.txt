LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
file(COPY ${CMAKE_BINARY_DIR}/cetpkg_variable_report DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include(NodeJSAddon)

add_subdirectory(conftool)
