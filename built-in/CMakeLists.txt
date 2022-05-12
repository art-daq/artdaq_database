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
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib_except/cetlib_except/exception.cc
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
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib_except/cetlib_except
    EXCLUDE
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib_except/cetlib_except/demangle.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib_except/cetlib_except/exception_collector.cc
  ) 
ENDIF()

IF(CETLIB_IS_STATIC)
  MESSAGE (STATUS "Making cetlib static")
  
  set(SOURCES
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/canonical_number.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/canonical_string.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/include.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/includer.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/filepath_maker.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/base_converter.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/search_path.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/filesystem.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/getenv.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/split_by_regex.cc
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
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib
    EXCLUDE
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/BasicPluginFactory.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/column_width.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/cpu_timer.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/crc32.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/get-loadable-libraries.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/inc-expand.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/LibraryManager.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/lpad.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/MD5Digest.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/nybbler.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/PluginFactory.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/replace_all.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/rpad.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/sha1.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/simple_stats.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/cetlib/cetlib/split_path.cc
  ) 
ENDIF()

IF(FHICLCPP_IS_STATIC)
  MESSAGE (STATUS "Making FHiCL static")
  
  set(SOURCES
    ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp/fhiclcpp/exception.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp/fhiclcpp/extended_value.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp/fhiclcpp/intermediate_table.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp/fhiclcpp/parse.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp/fhiclcpp/parse_shims.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp/fhiclcpp/Protection.cc
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
    ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp/fhiclcpp
    EXCLUDE
    ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp/fhiclcpp/ParameterSet.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp/fhiclcpp/ParameterSetID.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp/fhiclcpp/ParameterSetRegistry.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp/fhiclcpp/DatabaseSupport.cc
    ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp/fhiclcpp/coding.cc  
    ${TOP_CMAKE_SOURCE_DIR}/built-in/fhicl-cpp/fhiclcpp/make_ParameterSet.cc  
  ) 
ENDIF()


#install(DIRECTORY paramiko/build/lib DESTINATION ${flavorqual_dir})

