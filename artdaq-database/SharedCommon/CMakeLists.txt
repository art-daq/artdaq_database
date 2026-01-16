cet_make_library(
	SOURCE
	fileststem_functions.cpp
	helper_functions.cpp
	printStackTrace.cpp
	returned_result.cpp
  LIBRARIES
  Boost::filesystem
  TRACE::TRACE
  cetlib_except::cetlib_except
  -ldl
  )

install_headers()
install_source()
