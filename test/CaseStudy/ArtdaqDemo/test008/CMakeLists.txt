SET ( TEST_NUMBER "008" )

SET ( TEST_NAME "ArtdaqDemo_load" )
SET ( TEST_DBPROVIDER "filesystem" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5265.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_008_01_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o load -c artdaq_Aggregator1  -x Aggregator_topol.fnal.gov_5265.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_008_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5266.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_008_02_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o load -c artdaq_Aggregator2  -x Aggregator_topol.fnal.gov_5266.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_008_02_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_008_03_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o load -c artdaq_BoardReader1  -x BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_008_03_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_008_04_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o load -c artdaq_BoardReader2  -x BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_008_04_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/EventBuilder_topol.fnal.gov_5235.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_008_05_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o load -c artdaq_EventBuilder1  -x EventBuilder_topol.fnal.gov_5235.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_008_05_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5236.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/EventBuilder_topol.fnal.gov_5236.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_008_06_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o load -c artdaq_EventBuilder2  -x EventBuilder_topol.fnal.gov_5236.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_008_06_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")

SET ( TEST_DBPROVIDER "mongo" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5265.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_008_11_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o load -c artdaq_Aggregator1  -x Aggregator_topol.fnal.gov_5265.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_008_11_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5266.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_008_12_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o load -c artdaq_Aggregator2  -x Aggregator_topol.fnal.gov_5266.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_008_12_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_008_13_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o load -c artdaq_BoardReader1  -x BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_008_13_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_008_14_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o load -c artdaq_BoardReader2  -x BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_008_14_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/EventBuilder_topol.fnal.gov_5235.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_008_15_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o load -c artdaq_EventBuilder1  -x EventBuilder_topol.fnal.gov_5235.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_008_15_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5236.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/EventBuilder_topol.fnal.gov_5236.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_008_16_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o load -c artdaq_EventBuilder2  -x EventBuilder_topol.fnal.gov_5236.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_008_16_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")
