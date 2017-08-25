#add_subdirectory(mongo-cxx)

# cet_make( [LIBRARY_NAME <library name>]
#           [LIBRARIES <library link list>]
#           [SUBDIRS <source subdirectory>] (e.g., detail)
#           [USE_PRODUCT_NAME]
#           [EXCLUDE <ignore these files>] )

cet_make(
  LIBRARY_NAME 
  ${FHICLCPP}
  LIBRARIES
  cetlib
  cetlib_except
  ${SQLITE3}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_REGEX_LIBRARY}
  WITH_STATIC_LIBRARY
  SUBDIRS
  fhicl-cpp/fhiclcpp
  fhicl-cpp/fhiclcpp/detail
  fhicl-cpp/fhiclcpp/types
  fhicl-cpp/fhiclcpp/types/detail
  ) 
