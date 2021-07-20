#add_subdirectory(mongo-cxx)

# cet_make( [LIBRARY_NAME <library name>]
#           [LIBRARIES <library link list>]
#           [SUBDIRS <source subdirectory>] (e.g., detail)
#           [USE_PRODUCT_NAME]
#           [EXCLUDE <ignore these files>] )

cet_add_compiler_flags(-fvisibility=hidden)

IF(CETLIBEXCEPT_IS_STATIC)
  MESSAGE (STATUS "Making cetlib_except static")
  
  set(SOURCES
    cetlib_except/cetlib_except/exception.cc
    )
    
  set(LIBRARIES  
    )
    
  add_library(${CETLIB_EXCEPT_TARGET} OBJECT ${SOURCES})
  set_property(TARGET ${CETLIB_EXCEPT_TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
  target_link_libraries(${CETLIB_EXCEPT_TARGET} ${LIBRARIES})  

else()
  cet_make(
    LIBRARY_NAME 
    cetlib_except
    LIBRARIES
    WITH_STATIC_LIBRARY
    SUBDIRS
    cetlib_except/cetlib_except
    EXCLUDE
    cetlib_except/cetlib_except/demangle.cc
    cetlib_except/cetlib_except/exception_collector.cc
  ) 
ENDIF()

IF(CETLIB_IS_STATIC)
  MESSAGE (STATUS "Making cetlib static")
  
  set(SOURCES
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
    )

  set(LIBRARIES  
    cetlib_except
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    )

  include_directories( ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib )
    
  add_library(${CETLIB_TARGET} OBJECT ${SOURCES})
  set_property(TARGET ${CETLIB_TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
  target_link_libraries(${CETLIB_TARGET} ${LIBRARIES})  

else()
  cet_make(
    LIBRARY_NAME 
    cetlib
    LIBRARIES
    cetlib_except
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

  set(LIBRARIES  
    cetlib
    cetlib_except
    )
    
  include_directories( ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp )
    
  add_library(${FHICLCPP_TARGET} OBJECT ${SOURCES})
  set_property(TARGET ${FHICLCPP_TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
  target_link_libraries(${FHICLCPP_TARGET} ${LIBRARIES})  

else()
  cet_make(
    LIBRARY_NAME 
    fhiclcpp
    LIBRARIES
    cetlib
    cetlib_except
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

