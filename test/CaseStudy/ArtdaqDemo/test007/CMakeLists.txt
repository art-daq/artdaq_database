SET ( TEST_NUMBER "007" )

SET ( TEST_NAME "ArtdaqDemo_store" )
SET ( TEST_DBPROVIDER "filesystem" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5265.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5265.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o store -c artdaq_Aggregator1 -s Aggregator_topol.fnal.gov_5265.src.fcl -x Aggregator_topol.fnal.gov_5265.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5266.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5266.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_02_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o store -c artdaq_Aggregator2 -s Aggregator_topol.fnal.gov_5266.src.fcl -x Aggregator_topol.fnal.gov_5266.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_02_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_03_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o store -c artdaq_BoardReader1 -s BoardReader_TOY1_topol.fnal.gov_5205.src.fcl -x BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_03_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_04_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o store -c artdaq_BoardReader2 -s BoardReader_TOY2_topol.fnal.gov_5206.src.fcl -x BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_04_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/EventBuilder_topol.fnal.gov_5235.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/EventBuilder_topol.fnal.gov_5235.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_05_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o store -c artdaq_EventBuilder1 -s EventBuilder_topol.fnal.gov_5235.src.fcl -x EventBuilder_topol.fnal.gov_5235.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_05_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5236.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/EventBuilder_topol.fnal.gov_5236.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5236.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/EventBuilder_topol.fnal.gov_5236.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_06_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o store -c artdaq_EventBuilder2 -s EventBuilder_topol.fnal.gov_5236.src.fcl -x EventBuilder_topol.fnal.gov_5236.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_06_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")

SET ( TEST_DBPROVIDER "mongo" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5265.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5265.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_11_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o store -c artdaq_Aggregator1 -s Aggregator_topol.fnal.gov_5265.src.fcl -x Aggregator_topol.fnal.gov_5265.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_11_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5266.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5266.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_12_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o store -c artdaq_Aggregator2 -s Aggregator_topol.fnal.gov_5266.src.fcl -x Aggregator_topol.fnal.gov_5266.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_12_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_13_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o store -c artdaq_BoardReader1 -s BoardReader_TOY1_topol.fnal.gov_5205.src.fcl -x BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_13_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_14_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o store -c artdaq_BoardReader2 -s BoardReader_TOY2_topol.fnal.gov_5206.src.fcl -x BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_14_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/EventBuilder_topol.fnal.gov_5235.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/EventBuilder_topol.fnal.gov_5235.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_15_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o store -c artdaq_EventBuilder1 -s EventBuilder_topol.fnal.gov_5235.src.fcl -x EventBuilder_topol.fnal.gov_5235.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_15_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5236.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/EventBuilder_topol.fnal.gov_5236.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5236.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/EventBuilder_topol.fnal.gov_5236.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_16_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ArtdaqDemo_t -p ${TEST_DBPROVIDER} -o store -c artdaq_EventBuilder2 -s EventBuilder_topol.fnal.gov_5236.src.fcl -x EventBuilder_topol.fnal.gov_5236.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_16_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")
