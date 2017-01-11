cet_make(
  LIBRARIES
  artdaq-database_SharedCommon
)


add_subdirectory(MongoDB)
add_subdirectory(FileSystemDB)
add_subdirectory(UcondDB)

install_headers()
install_source()
