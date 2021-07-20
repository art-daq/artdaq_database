cet_make(
  LIBRARIES
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}      
  cetlib_except
  -ldl
  )
  if(CETLIBEXCEPT_IS_STATIC)
  add_dependencies(artdaq-database_SharedCommon ${CETLIB_EXCEPT_TARGET})
  endif()

install_headers()
install_source()
