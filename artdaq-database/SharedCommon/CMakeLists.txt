cet_make_library(
	SOURCE
	fileststem_functions.cpp
	helper_functions.cpp
	printStackTrace.cpp
	returned_result.cpp
  LIBRARIES
  Boost::filesystem
  TRACE::TRACE
  cetlib_except
  -ldl
  )
  if(CETLIBEXCEPT_IS_STATIC)
  add_dependencies(artdaq-database_SharedCommon ${CETLIB_EXCEPT_TARGET})
  endif()

install_headers()
install_source()
