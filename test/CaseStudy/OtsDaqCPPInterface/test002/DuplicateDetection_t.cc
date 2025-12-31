#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (DuplicateDetection test)

#include <boost/test/unit_test.hpp>

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

#include "../DatabaseConfigurationInterface.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

#include "artdaq-database/ConfigurationDB/Multitasker.h"
#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/dispatch_common.h"

bool jsonContains(const std::string& json, const std::string& key, const std::string& value) {
  std::string pattern1 = "\"" + key + "\":\"" + value + "\"";
  std::string pattern2 = "\"" + key + "\" : \"" + value + "\"";
  return json.find(pattern1) != std::string::npos || json.find(pattern2) != std::string::npos;
}

struct DuplicateTestData {
  DuplicateTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== DUPLICATE DETECTION TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~DuplicateTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  void updateConfigCount(int count) { oldConfigCount = count; }

  int baseVersion;
  int oldConfigCount = 0;
};

using namespace ots;

DuplicateTestData fixture;

BOOST_AUTO_TEST_SUITE(duplicate_detection_test)

BOOST_AUTO_TEST_CASE(duplicate_version_rejected) {
  std::cout << "\n=== TEST 1: Duplicate version rejected ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();

  int testVersion = fixture.baseVersion;
  cfg1->getView().version = testVersion;
  cfg1->getView().fillFromJSON("{\"test\": \"first_save\"}");

  std::cout << "Step 1: Saving configuration version " << testVersion << " (first time)...\n";

  auto result1 = ifc.saveActiveVersion(cfg1.get());

  BOOST_CHECK_EQUAL(result1, 0);
  std::cout << "  Result: SUCCESS (as expected)\n";

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration001>();

  cfg2->getView().version = testVersion;
  cfg2->getView().fillFromJSON("{\"test\": \"second_save\"}");

  std::cout << "Step 2: Attempting to save DUPLICATE version " << testVersion << " (second time)...\n";

  auto result2 = ifc.saveActiveVersion(cfg2.get());

  BOOST_CHECK_NE(result2, 0);
  std::cout << "  Result: FAILURE (as expected - duplicate rejected)\n";

  std::shared_ptr<ConfigurationBase> cfgRead = std::make_shared<TestConfiguration001>();

  auto resultRead = ifc.fill(cfgRead.get(), testVersion);
  BOOST_CHECK_EQUAL(resultRead, 0);

  BOOST_CHECK_MESSAGE(jsonContains(cfgRead->getView()._json, "test", "first_save"), "Expected 'first_save' but got: " << cfgRead->getView()._json);

  std::cout << "  Verified: Original version preserved\n";
}

BOOST_AUTO_TEST_CASE(duplicate_version_allowed_with_overwrite) {
  std::cout << "\n=== TEST 2: Duplicate version allowed with overwrite ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int testVersion = fixture.baseVersion + 1;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = testVersion;
  cfg1->getView().fillFromJSON("{\"data\": \"original\"}");

  std::cout << "Step 1: Saving original version " << testVersion << "\n";
  auto result1 = ifc.saveActiveVersion(cfg1.get());
  BOOST_CHECK_EQUAL(result1, 0);

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration001>();
  cfg2->getView().version = testVersion;
  cfg2->getView().fillFromJSON("{\"data\": \"updated\"}");

  std::cout << "Step 2: Overwriting version " << testVersion << " with overwrite=true\n";
  auto result2 = ifc.saveActiveVersion(cfg2.get(), true);

  BOOST_CHECK_EQUAL(result2, 0);
  std::cout << "  Result: SUCCESS (overwrite allowed)\n";

  std::shared_ptr<ConfigurationBase> cfgRead = std::make_shared<TestConfiguration001>();
  auto resultRead = ifc.fill(cfgRead.get(), testVersion);
  BOOST_CHECK_EQUAL(resultRead, 0);

  BOOST_CHECK_MESSAGE(jsonContains(cfgRead->getView()._json, "data", "updated"), "Expected 'updated' but got: " << cfgRead->getView()._json);
  std::cout << "  Verified: Version successfully overwritten\n";
}

BOOST_AUTO_TEST_CASE(duplicate_composition_rejected) {
  std::cout << "\n=== TEST 3: Duplicate composition rejected ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 10;
  int version2 = fixture.baseVersion + 20;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  auto result1 = ifc.saveActiveVersion(cfg1.get());
  BOOST_CHECK_EQUAL(result1, 0);

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = version2;
  auto result2 = ifc.saveActiveVersion(cfg2.get());
  BOOST_CHECK_EQUAL(result2, 0);

  std::cout << "Created versions: " << version1 << ", " << version2 << "\n";

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;
  compositionMap[cfg2->getConfigurationName()] = version2;

  std::string compositionName = std::string{"TestComposition_"} + std::to_string(fixture.baseVersion);

  std::cout << "Step 2: Creating composition '" << compositionName << "' (first time)...\n";

  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(compositionMap, compositionName));
  std::cout << "  Result: SUCCESS\n";

  std::cout << "Step 3: Attempting to create DUPLICATE composition '" << compositionName << "' (second time)...\n";

  BOOST_CHECK_THROW(ifc.storeGlobalConfiguration(compositionMap, compositionName), std::runtime_error);
  std::cout << "  Result: FAILURE (as expected - duplicate rejected)\n";

  auto loadedMap = ifc.loadGlobalConfiguration(compositionName);

  BOOST_CHECK_EQUAL(loadedMap.size(), 2);
  BOOST_CHECK_EQUAL(loadedMap[cfg1->getConfigurationName()], version1);
  BOOST_CHECK_EQUAL(loadedMap[cfg2->getConfigurationName()], version2);

  std::cout << "  Verified: Original composition preserved\n";
}

BOOST_AUTO_TEST_CASE(different_composition_names_allowed) {
  std::cout << "\n=== TEST 4: Different composition names allowed ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 100;
  int version2 = fixture.baseVersion + 200;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = version2;
  ifc.saveActiveVersion(cfg2.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;
  compositionMap[cfg2->getConfigurationName()] = version2;

  std::string compositionName1 = std::string{"Composition_A_"} + std::to_string(fixture.baseVersion);

  std::cout << "Creating composition '" << compositionName1 << "'\n";
  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(compositionMap, compositionName1));

  std::string compositionName2 = std::string{"Composition_B_"} + std::to_string(fixture.baseVersion);

  std::cout << "Creating composition '" << compositionName2 << "' (same members, different name)\n";

  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(compositionMap, compositionName2));
  std::cout << "  Result: SUCCESS (different names allowed)\n";

  auto configs = ifc.findAllGlobalConfigurations();

  bool found1 = (std::find(configs.begin(), configs.end(), compositionName1) != configs.end());
  bool found2 = (std::find(configs.begin(), configs.end(), compositionName2) != configs.end());

  BOOST_CHECK_EQUAL(found1, true);
  BOOST_CHECK_EQUAL(found2, true);

  std::cout << "  Verified: Both compositions exist independently\n";
}

BOOST_AUTO_TEST_CASE(composition_different_members_allowed) {
  std::cout << "\n=== TEST 5: Composition different members allowed ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 300;
  int version2 = fixture.baseVersion + 400;
  int version3 = fixture.baseVersion + 500;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::shared_ptr<ConfigurationBase> cfg2a = std::make_shared<TestConfiguration002>();
  cfg2a->getView().version = version2;
  ifc.saveActiveVersion(cfg2a.get());

  std::shared_ptr<ConfigurationBase> cfg2b = std::make_shared<TestConfiguration002>();
  cfg2b->getView().version = version3;
  ifc.saveActiveVersion(cfg2b.get());

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[cfg1->getConfigurationName()] = version1;
  map1[cfg2a->getConfigurationName()] = version2;

  std::string compName1 = std::string{"CompA_"} + std::to_string(fixture.baseVersion);

  ifc.storeGlobalConfiguration(map1, compName1);
  std::cout << "Created composition '" << compName1 << "' with versions " << version1 << ", " << version2 << "\n";

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[cfg1->getConfigurationName()] = version1;
  map2[cfg2b->getConfigurationName()] = version3;

  std::string compName2 = std::string{"CompB_"} + std::to_string(fixture.baseVersion);

  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(map2, compName2));
  std::cout << "Created composition '" << compName2 << "' with versions " << version1 << ", " << version3 << " (SUCCESS)\n";
}

BOOST_AUTO_TEST_SUITE_END()
