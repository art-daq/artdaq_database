cet_enable_asserts()


IF (FORMAT_WITH_CLANG)
  string(TOUPPER ${FORMAT_WITH_CLANG} FORMAT_WITH_CLANG_UC )
  if( ${FORMAT_WITH_CLANG_UC} MATCHES "TRUE" )
    include(clang-format)
  endif()
ENDIF()

# Make sure tests have correct environment settings.
include(CetTest)


string(FIND "x${artdaq_database_EXEC_PREFIX}" "ots" OTS_FOUND)
if(${OTS_FOUND} GREATER -1)
  MESSAGE(STATUS "DISABLE built-in FHICLCPP")
  set(USE_FHICLCPP FALSE)
endif()

if(DEFINED ENV{USE_FHICLCPP})
  if($ENV{USE_FHICLCPP} STREQUAL "0")
    MESSAGE( STATUS "Disable built-in FHICLCPP")
    set(USE_FHICLCPP FALSE)
  else()
    MESSAGE( STATUS "Enable built-in FHICLCPP")
    set(USE_FHICLCPP TRUE)
  endif()
endif()

if(NOT DEFINED USE_FHICLCPP)
    MESSAGE( STATUS "Enable built-in FHICLCPP")
  set(USE_FHICLCPP TRUE)
endif()

if(USE_FHICLCPP)
  include_directories(${TOP_CMAKE_BINARY_DIR}/built-in/fhicl-cpp)
  include_directories(${TOP_CMAKE_BINARY_DIR}/built-in/cetlib)
  include_directories(${TOP_CMAKE_BINARY_DIR}/built-in/cetlib_except)
 
endif()

set(ARTDAQ_DATABASE_URI_FILESYSTEM "filesystemdb://${CMAKE_BINARY_DIR}/databases/filesystemdb/test_db")
set(ARTDAQ_DATABASE_URI_MONGO "mongodb://127.0.0.1:27017/test_db")

set(ARTDAQ_DATABASE_URI_UCOND $ENV{ARTDAQ_DATABASE_URI_UCOND})
set(ARTDAQ_DATABASE_AUTH $ENV{ARTDAQ_DATABASE_AUTH})

cet_test_env("FHICL_FILE_PATH=.")

cet_test_env("PATH=${TOP_CMAKE_BINARY_DIR}/bin:$ENV{PATH}")
cet_test_env("LD_LIBRARY_PATH=${TOP_CMAKE_BINARY_DIR}/lib:$ENV{LD_LIBRARY_PATH}")

find_program(mongod_bin name mongod HINTS $ENV{HOME} $ENV{MONGODB_FQ_DIR})

configure_file("CTestCustom.cmake.in" ${CMAKE_BINARY_DIR}/CTestCustom.cmake)

#test order is important
add_subdirectory(JsonDocument)
add_subdirectory(Overlay)
add_subdirectory(DataFormats)
add_subdirectory(StorageProviders)

add_subdirectory(ConfigurationDB)
add_subdirectory(CaseStudy)

