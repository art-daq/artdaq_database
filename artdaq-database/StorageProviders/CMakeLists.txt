cet_make(
  LIBRARIES
  artdaq-database_SharedCommon
  ${CETLIB_EXCEPT}
)


add_subdirectory(MongoDB)
add_subdirectory(FileSystemDB)
add_subdirectory(UconDB)

install_headers()
install_source()
