#add_subdirectory(mongo-cxx)

# cet_make( [LIBRARY_NAME <library name>]
#           [LIBRARIES <library link list>]
#           [SUBDIRS <source subdirectory>] (e.g., detail)
#           [USE_PRODUCT_NAME]
#           [EXCLUDE <ignore these files>] )


SET(MAKE_FHICLCPP_STATIC $ENV{MAKE_FHICLCPP_STATIC})
IF(MAKE_FHICLCPP_STATIC)
  string(TOUPPER ${MAKE_FHICLCPP_STATIC} MAKE_FHICLCPP_STATIC_UC )
  if( ${MAKE_FHICLCPP_STATIC_UC} MATCHES "TRUE" )
    set(FHICLCPP_IS_STATIC "TRUE")
  endif()
ENDIF()

IF(FHICLCPP_IS_STATIC)
  MESSAGE (STATUS "Making FHiCL static")
  
  set(SOURCES
    fhicl-cpp/fhiclcpp/exception.cc
    fhicl-cpp/fhiclcpp/extended_value.cc
    fhicl-cpp/fhiclcpp/intermediate_table.cc
    fhicl-cpp/fhiclcpp/parse.cc
    fhicl-cpp/fhiclcpp/parse_shims.cc
    fhicl-cpp/fhiclcpp/Protection.cc
    )

  find_library(CETLIB NAMES cetlib HINTS $ENV{CETLIB_LIB} )
  find_library(CETLIBEXCEPT NAMES cetlib_except HINTS $ENV{CETLIB_EXCEPT_LIB} )
    
  set(LIBRARIES  
    ${CETLIB}
    ${CETLIBEXCEPT}
    #${SQLITE3}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    )
    
  add_library(${FHICLCPP} STATIC ${SOURCES})
  set_property(TARGET ${FHICLCPP} PROPERTY POSITION_INDEPENDENT_CODE ON)
  target_link_libraries(${FHICLCPP} ${LIBRARIES})  
else()
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
    EXCLUDE
    fhicl-cpp/fhiclcpp/ParameterSet.cc
    fhicl-cpp/fhiclcpp/ParameterSetID.cc
    fhicl-cpp/fhiclcpp/ParameterSetRegistry.cc
    fhicl-cpp/fhiclcpp/DatabaseSupport.cc
    fhicl-cpp/fhiclcpp/coding.cc  
    fhicl-cpp/fhiclcpp/make_ParameterSet.cc  
  ) 
ENDIF()

#install(DIRECTORY paramiko/build/lib DESTINATION ${flavorqual_dir})

