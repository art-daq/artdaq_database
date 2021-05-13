#add_subdirectory(mongo-cxx)

# cet_make( [LIBRARY_NAME <library name>]
#           [LIBRARIES <library link list>]
#           [SUBDIRS <source subdirectory>] (e.g., detail)
#           [USE_PRODUCT_NAME]
#           [EXCLUDE <ignore these files>] )

cet_add_compiler_flags(-fvisibility=hidden)

SET(MAKE_CETLIBEXCEPT_STATIC $ENV{MAKE_CETLIBEXCEPT_STATIC})
IF(MAKE_CETLIBEXCEPT_STATIC)
  string(TOUPPER ${MAKE_CETLIBEXCEPT_STATIC} MAKE_CETLIBEXCEPT_STATIC_UC )
  if( ${MAKE_CETLIBEXCEPT_STATIC_UC} MATCHES "TRUE" )
    set(CETLIBEXCEPT_IS_STATIC "TRUE")
  endif()
ELSE()
   set(CETLIBEXCEPT_IS_STATIC "TRUE")
ENDIF()

IF(CETLIBEXCEPT_IS_STATIC)
  MESSAGE (STATUS "Making cetlib_except static")
  cet_make_library(LIBRARY_NAME ${CETLIB_EXCEPT} STATIC SOURCE 
    cetlib_except/cetlib_except/exception.cc
    )
    
  set_property(TARGET ${CETLIB_EXCEPT} PROPERTY POSITION_INDEPENDENT_CODE ON)

else()
  cet_make(
    LIBRARY_NAME 
    ${CETLIB_EXCEPT}
    LIBRARIES
    WITH_STATIC_LIBRARY
    SUBDIRS
    cetlib_except/cetlib_except
    EXCLUDE
    cetlib_except/cetlib_except/demangle.cc
    cetlib_except/cetlib_except/exception_collector.cc
  ) 
ENDIF()

SET(MAKE_CETLIB_STATIC $ENV{MAKE_CETLIB_STATIC})
IF(MAKE_CETLIB_STATIC)
  string(TOUPPER ${MAKE_CETLIB_STATIC} MAKE_CETLIB_STATIC_UC )
  if( ${MAKE_CETLIB_STATIC_UC} MATCHES "TRUE" )
    set(CETLIB_IS_STATIC "TRUE")
  endif()
ELSE()
   set(CETLIB_IS_STATIC "TRUE")
ENDIF()

IF(CETLIB_IS_STATIC)
  MESSAGE (STATUS "Making cetlib static")
  
  cet_make_library(LIBRARY_NAME ${CETLIB} STATIC SOURCE 
    cetlib/cetlib/canonical_number.cc
    cetlib/cetlib/canonical_string.cc
    cetlib/cetlib/include.cc
    cetlib/cetlib/includer.cc
    cetlib/cetlib/filepath_maker.cc
    cetlib/cetlib/base_converter.cc
    cetlib/cetlib/search_path.cc
    cetlib/cetlib/filesystem.cc
    cetlib/cetlib/getenv.cc
    cetlib/cetlib/split_by_regex.cc
    LIBRARIES  
    ${CETLIB_EXCEPT_LIB}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    )
        
  set_property(TARGET ${CETLIB} PROPERTY POSITION_INDEPENDENT_CODE ON)

else()
  cet_make(
    LIBRARY_NAME 
    ${CETLIB}
    LIBRARIES
    ${CETLIB_EXCEPT}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    WITH_STATIC_LIBRARY
    SUBDIRS
    cetlib/cetlib
    EXCLUDE
    cetlib/cetlib/BasicPluginFactory.cc
    cetlib/cetlib/column_width.cc
    cetlib/cetlib/cpu_timer.cc
    cetlib/cetlib/crc32.cc
    cetlib/cetlib/get-loadable-libraries.cc
    cetlib/cetlib/inc-expand.cc
    cetlib/cetlib/LibraryManager.cc
    cetlib/cetlib/lpad.cc
    cetlib/cetlib/MD5Digest.cc
    cetlib/cetlib/nybbler.cc
    cetlib/cetlib/PluginFactory.cc
    cetlib/cetlib/replace_all.cc
    cetlib/cetlib/rpad.cc
    cetlib/cetlib/sha1.cc
    cetlib/cetlib/simple_stats.cc
    cetlib/cetlib/split_path.cc
  ) 
ENDIF()

SET(MAKE_FHICLCPP_STATIC $ENV{MAKE_FHICLCPP_STATIC})
IF(MAKE_FHICLCPP_STATIC)
  string(TOUPPER ${MAKE_FHICLCPP_STATIC} MAKE_FHICLCPP_STATIC_UC )
  if( ${MAKE_FHICLCPP_STATIC_UC} MATCHES "TRUE" )
    set(FHICLCPP_IS_STATIC "TRUE")
  endif()
ELSE()
	set(FHICLCPP_IS_STATIC "TRUE")
ENDIF()

IF(FHICLCPP_IS_STATIC)
  MESSAGE (STATUS "Making FHiCL static")
  
  cet_make_library(LIBRARY_NAME ${FHICLCPP} STATIC SOURCE 
    fhicl-cpp/fhiclcpp/exception.cc
    fhicl-cpp/fhiclcpp/extended_value.cc
    fhicl-cpp/fhiclcpp/intermediate_table.cc
    fhicl-cpp/fhiclcpp/parse.cc
    fhicl-cpp/fhiclcpp/parse_shims.cc
    fhicl-cpp/fhiclcpp/Protection.cc
   LIBRARIES  
    ${CETLIB_LIB}
    ${CETLIB_EXCEPT_LIB}
    )
  set_property(TARGET ${FHICLCPP} PROPERTY POSITION_INDEPENDENT_CODE ON)
else()
  cet_make(
    LIBRARY_NAME 
    ${FHICLCPP}
    LIBRARIES
    ${CETLIB}
    ${CETLIB_EXCEPT}
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

