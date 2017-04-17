#define BOOST_TEST_MODULE (databaseconfiguration test)

#include "boost/test/auto_unit_test.hpp"

#include <time.h> /* time */
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "../DatabaseConfigurationInterface.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/dispatch_common.h"

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/shared_literals.h"

struct TestData {
  TestData() {
#if 0
  artdaq::database::configuration::debug::ExportImport();
  artdaq::database::configuration::debug::ManageAliases();
  artdaq::database::configuration::debug::ManageConfigs();
  artdaq::database::configuration::debug::ManageDocuments();
  artdaq::database::configuration::debug::Metadata();

  artdaq::database::configuration::debug::detail::ExportImport();
  artdaq::database::configuration::debug::detail::ManageAliases();
  artdaq::database::configuration::debug::detail::ManageConfigs();
  artdaq::database::configuration::debug::detail::ManageDocuments();
  artdaq::database::configuration::debug::detail::Metadata();

  artdaq::database::configuration::debug::options::OperationBase();
  artdaq::database::configuration::debug::options::BulkOperations();
  artdaq::database::configuration::debug::options::ManageDocuments();
  artdaq::database::configuration::debug::options::ManageConfigs();
  artdaq::database::configuration::debug::options::ManageAliases();

  artdaq::database::configuration::debug::MongoDB();
  artdaq::database::configuration::debug::UconDB();
  artdaq::database::configuration::debug::FileSystemDB();

  artdaq::database::filesystem::debug::enable();
  artdaq::database::mongo::debug::enable();

 // artdaq::database::docrecord::debug::JSONDocumentBuilder();
 // artdaq::database::docrecord::debug::JSONDocument();

  debug::registerUngracefullExitHandlers();
  artdaq::database::useFakeTime(true);
#endif
    std::cout << "setup fixture\n";
  }

  ~TestData() {
    std::cout << "setup fixture\n";
  }

  void updateConfigCount(int count) { _oldConfigCount = count; }

  int version() { return _version; }
  int oldConfigCount() { return _oldConfigCount; }

  const int _version = (srand(time(NULL)), rand() % 99999 + 100000);

  int _oldConfigCount = 0;
  
  int rand_version() {return (srand(time(NULL)), rand() % 88888 + 111111);}
};

using namespace ots;

TestData fixture;

BOOST_AUTO_TEST_SUITE(databaseconfiguration_test)

BOOST_AUTO_TEST_CASE(configure_tests) {
  std::cout << "TestData::version=" << fixture.version() << "\n";
  return;
}

BOOST_AUTO_TEST_CASE(write_document) {
  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();  
  auto ifc = DatabaseConfigurationInterface();

  cfg1->getView().version = fixture.version();
  auto result = ifc.saveActiveVersion(cfg1.get());

  BOOST_CHECK_EQUAL(result, 0);
  return;
}

BOOST_AUTO_TEST_CASE(read_document) {
  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  auto ifc = DatabaseConfigurationInterface();

  auto result = ifc.fill(cfg1.get(), fixture.version());

  BOOST_CHECK_EQUAL(result, 0);

  return;
}

BOOST_AUTO_TEST_CASE(store_configuration) {
  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();

  auto ifc = DatabaseConfigurationInterface();

  cfg1->getView().version = fixture.version() + 1;
  cfg2->getView().version = fixture.version() + 2;

  auto result1 = ifc.saveActiveVersion(cfg1.get());
  BOOST_CHECK_EQUAL(result1, 0);

  auto result2 = ifc.saveActiveVersion(cfg2.get());
  BOOST_CHECK_EQUAL(result2, 0);

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[cfg1->getConfigurationName()] = cfg1->getView().version;
  map[cfg2->getConfigurationName()] = cfg2->getView().version;

  auto configName = std::string{"config"} + std::to_string(fixture.version());

  auto list = ifc.findAllGlobalConfigurations();

  fixture.updateConfigCount(list.size());

  ifc.storeGlobalConfiguration(map, configName);

  return;
}

BOOST_AUTO_TEST_CASE(load_configuration) {
  auto ifc = DatabaseConfigurationInterface();

  auto configName = std::string{"config"} + std::to_string(fixture.version());

  auto map = ifc.loadGlobalConfiguration(configName);

  BOOST_CHECK_EQUAL(map.size(), 2);

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();

  BOOST_CHECK_EQUAL(map.at(cfg1->getConfigurationName()), fixture.version() + 1);
  BOOST_CHECK_EQUAL(map.at(cfg2->getConfigurationName()), fixture.version() + 2);

  return;
}

BOOST_AUTO_TEST_CASE(find_all_configurations) {
  auto ifc = DatabaseConfigurationInterface();

  auto list = ifc.findAllGlobalConfigurations();

  BOOST_CHECK_EQUAL(list.size(), fixture.oldConfigCount() + 1);

  auto configName = std::string{"config"} + std::to_string(fixture.version());

  auto found = (std::find(list.begin(), list.end(), configName) != list.end());

  BOOST_CHECK_EQUAL(found, true);

  return;
}

BOOST_AUTO_TEST_CASE(list_configuration_types) {
  auto ifc = DatabaseConfigurationInterface();

  auto list = ifc.listConfigurationsTypes();

  BOOST_CHECK_EQUAL(list.size(), 2);

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();

  auto found1 = (std::find(list.begin(), list.end(), cfg1->getConfigurationName()) != list.end());

  BOOST_CHECK_EQUAL(found1, true);

  auto found2 = (std::find(list.begin(), list.end(), cfg2->getConfigurationName()) != list.end());

  BOOST_CHECK_EQUAL(found2, true);

  return;
}

BOOST_AUTO_TEST_CASE(find_configuration_version) {
  auto ifc = DatabaseConfigurationInterface();

   std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();

   auto list = ifc.getVersions(cfg1.get());

   auto found1 = (std::find(list.begin(), list.end(), fixture.version()) != list.end());

   BOOST_CHECK_EQUAL(found1, true);
   
   auto found2 = (std::find(list.begin(), list.end(), fixture.version() + 1) != list.end());

   BOOST_CHECK_EQUAL(found2, true);

   return;
}

BOOST_AUTO_TEST_CASE(find_latest_configuration_version) {
  auto ifc = DatabaseConfigurationInterface();

   std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();

   auto version = ifc.findLatestVersion(cfg1.get());

   auto list = ifc.getVersions(cfg1.get());

   std::cout << "Found versions\n";
   
   for(auto version:list) { std::cout << std::to_string(version) << ", "; }
      
   std::cout << "\nGot latest version" <<  std::to_string(version) << "\n";

   return;
}

BOOST_AUTO_TEST_CASE(overwrite_document) {
  auto ifc = DatabaseConfigurationInterface();

  auto oldJson=std::string{"{\n\"oldJson\" : 321\n}"};
  auto version=fixture.rand_version();
  std::shared_ptr<ConfigurationBase> w1 = std::make_shared<TestConfiguration001>();  
  w1->getView().fillFromJSON(oldJson);
  w1->getView().version = version;
  auto result = ifc.saveActiveVersion(w1.get());
  BOOST_CHECK_EQUAL(result, 0);
  
  std::shared_ptr<ConfigurationBase> r1 = std::make_shared<TestConfiguration001>();
  result = ifc.fill(r1.get(), version);
  BOOST_CHECK_EQUAL(result, 0);
  BOOST_CHECK_EQUAL(r1->getView()._json,oldJson);  

  std::shared_ptr<ConfigurationBase> w2 = std::make_shared<TestConfiguration001>();
  w2->getView().version = version;
  result = ifc.saveActiveVersion(w2.get(),true);
  BOOST_CHECK_EQUAL(result, 0);

  r1 = std::make_shared<TestConfiguration001>();
  result = ifc.fill(r1.get(), version);
  BOOST_CHECK_EQUAL(result, 0);
  BOOST_CHECK_EQUAL(r1->getView()._json,w2->getView()._json);  
  
  std::shared_ptr<ConfigurationBase> r2 = std::make_shared<TestConfiguration001>();
  result = ifc.fill(r2.get(), version);
  BOOST_CHECK_EQUAL(result, 0);
  BOOST_CHECK_EQUAL(r2->getView()._json,w2->getView()._json);  

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  BOOST_CHECK_EQUAL(r2->getView()._json,cfg->getView()._json);  
 
  return;
}

BOOST_AUTO_TEST_SUITE_END()
