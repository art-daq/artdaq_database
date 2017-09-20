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
  #${SQLITE3}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_REGEX_LIBRARY}
  WITH_STATIC_LIBRARY
  SUBDIRS
  fhicl-cpp/fhiclcpp
  fhicl-cpp/fhiclcpp/detail
  fhicl-cpp/fhiclcpp/types
  fhicl-cpp/fhiclcpp/types/detail
  EXCLUDE
  fhicl-cpp/fhiclcpp/ParameterSet.cc
  fhicl-cpp/fhiclcpp/ParameterSetID.cc
  fhicl-cpp/fhiclcpp/ParameterSetRegistry.cc
  fhicl-cpp/fhiclcpp/DatabaseSupport.cc
  fhicl-cpp/fhiclcpp/coding.cc  
  fhicl-cpp/fhiclcpp/make_ParameterSet.cc  
  fhicl-cpp/fhiclcpp/types/DelegatedParameter.cc
  fhicl-cpp/fhiclcpp/types/OptionalDelegatedParameter.cc
  fhicl-cpp/fhiclcpp/types/Name.cc  
  fhicl-cpp/fhiclcpp/types/detail/ValidateThenSet.cc
  fhicl-cpp/fhiclcpp/types/detail/SearchAllowedConfiguration.cc
  fhicl-cpp/fhiclcpp/types/detail/PrintAllowedConfiguration.cc
  fhicl-cpp/fhiclcpp/types/detail/NameStackRegistry.cc
  fhicl-cpp/fhiclcpp/types/detail/MaybeDisplayParent.cc
  fhicl-cpp/fhiclcpp/detail/encode_extended_value.cc
  fhicl-cpp/fhiclcpp/detail/Prettifier.cc
  fhicl-cpp/fhiclcpp/detail/PrettifierAnnotated.cc
  fhicl-cpp/fhiclcpp/detail/PrettifierPrefixAnnotated.cc
  fhicl-cpp/fhiclcpp/detail/KeyAssembler.cc
) 
