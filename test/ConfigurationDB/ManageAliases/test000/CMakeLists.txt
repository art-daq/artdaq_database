SET ( TEST_NUMBER "000" )
SET ( TEST_NAME "seeddatacab" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5265.fcl COPYONLY)

# Register test with CTest
add_test(NAME conftool_mongo_store_${TEST_NUMBER}_01_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/conftool -p mongo -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version004 -e ${TEST_ENTITY_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_mongo_store_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5266.fcl COPYONLY)

# Register test with CTest
add_test(NAME conftool_mongo_store_${TEST_NUMBER}_02_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/conftool -p mongo -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5266.fcl -f fhicl -v version002 -e ${TEST_ENTITY_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_mongo_store_${TEST_NUMBER}_02_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5265.fcl COPYONLY)

# Register test with CTest
add_test(NAME conftool_mongo_store_${TEST_NUMBER}_03_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/conftool -p mongo -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version003 -e ${TEST_ENTITY_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_mongo_store_${TEST_NUMBER}_03_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5266.fcl COPYONLY)

# Register test with CTest
add_test(NAME conftool_mongo_store_${TEST_NUMBER}_04_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/conftool -p mongo -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5266.fcl -f fhicl -v version001 -e ${TEST_ENTITY_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_mongo_store_${TEST_NUMBER}_04_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")






# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5265.fcl COPYONLY)

# Register test with CTest
add_test(NAME conftool_filesystem_store_${TEST_NUMBER}_01_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/conftool -p filesystem -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version004 -e ${TEST_ENTITY_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_filesystem_store_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5266.fcl COPYONLY)

# Register test with CTest
add_test(NAME conftool_filesystem_store_${TEST_NUMBER}_02_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/conftool -p filesystem -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5266.fcl -f fhicl -v version002 -e ${TEST_ENTITY_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_filesystem_store_${TEST_NUMBER}_02_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5265.fcl COPYONLY)

# Register test with CTest
add_test(NAME conftool_filesystem_store_${TEST_NUMBER}_03_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/conftool -p filesystem -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version003 -e ${TEST_ENTITY_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_filesystem_store_${TEST_NUMBER}_03_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/Aggregator_topol.fnal.gov_5266.fcl COPYONLY)

# Register test with CTest
add_test(NAME conftool_filesystem_store_${TEST_NUMBER}_04_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/conftool -p filesystem -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5266.fcl -f fhicl -v version001 -e ${TEST_ENTITY_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_filesystem_store_${TEST_NUMBER}_04_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")
