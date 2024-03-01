cet_make_library(
  SOURCE
  storage_providers.cpp
  LIBRARIES
  artdaq-database_SharedCommon
  cetlib_except
)


add_subdirectory(MongoDB)
add_subdirectory(FileSystemDB)
add_subdirectory(UconDB)

install_headers()
install_source()
