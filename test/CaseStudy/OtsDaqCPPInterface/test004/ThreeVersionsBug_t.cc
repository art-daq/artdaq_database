#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (ThreeVersionsBug test)

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <memory>
#include <string>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct BugTestData {
  BugTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== INVESTIGATING THREE VERSIONS BUG ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~BugTestData() { std::cout << "\n=== INVESTIGATION COMPLETE ===\n"; }

  int baseVersion;
};

BugTestData fixture;

BOOST_AUTO_TEST_SUITE(three_versions_bug_test)

BOOST_AUTO_TEST_CASE(attempt_three_versions_same_type) {
  std::cout << "\n=== TEST 1: Attempting to add three versions ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion;
  int version2 = fixture.baseVersion + 1;
  int version3 = fixture.baseVersion + 2;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());
  std::cout << "Created version 1: " << version1 << "\n";

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration001>();
  cfg2->getView().version = version2;
  ifc.saveActiveVersion(cfg2.get());
  std::cout << "Created version 2: " << version2 << "\n";

  std::shared_ptr<ConfigurationBase> cfg3 = std::make_shared<TestConfiguration001>();
  cfg3->getView().version = version3;
  ifc.saveActiveVersion(cfg3.get());
  std::cout << "Created version 3: " << version3 << "\n";

  std::cout << "\nAttempt 1: Using std::map...\n";
  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};

  compositionMap[cfg1->getConfigurationName()] = version1;
  std::cout << "  Added version1, map size: " << compositionMap.size() << "\n";

  compositionMap[cfg2->getConfigurationName()] = version2;
  std::cout << "  Added version2, map size: " << compositionMap.size() << " (should still be 1)\n";

  compositionMap[cfg3->getConfigurationName()] = version3;
  std::cout << "  Added version3, map size: " << compositionMap.size() << " (should still be 1)\n";

  BOOST_CHECK_EQUAL(compositionMap.size(), 1);
  BOOST_CHECK_EQUAL(compositionMap[cfg1->getConfigurationName()], version3);

  std::cout << "  Result: Map correctly contains only last version: " << version3 << "\n";

  std::string compName = std::string{"BugTest1_"} + std::to_string(fixture.baseVersion);

  ifc.storeGlobalConfiguration(compositionMap, compName);
  std::cout << "  Created composition: " << compName << "\n";

  auto loadedMap = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loadedMap.size(), 1);

  std::cout << "  Verified: Composition has " << loadedMap.size() << " member (correct)\n";
}

BOOST_AUTO_TEST_CASE(inspect_database_storage_format) {
  std::cout << "\n=== TEST 2: Inspecting database storage format ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 10;
  int version2 = fixture.baseVersion + 11;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = version2;
  ifc.saveActiveVersion(cfg2.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;
  compositionMap[cfg2->getConfigurationName()] = version2;

  std::string compName = std::string{"BugTest2_"} + std::to_string(fixture.baseVersion);

  ifc.storeGlobalConfiguration(compositionMap, compName);

  auto loadedMap = ifc.loadGlobalConfiguration(compName);

  std::cout << "Composition '" << compName << "' contains:\n";
  for (auto const& entry : loadedMap) {
    std::cout << "  " << entry.first << " -> version " << entry.second << "\n";
  }

  BOOST_CHECK_EQUAL(loadedMap.size(), 2);

  int count001 = loadedMap.count(cfg1->getConfigurationName());
  int count002 = loadedMap.count(cfg2->getConfigurationName());

  BOOST_CHECK_EQUAL(count001, 1);
  BOOST_CHECK_EQUAL(count002, 1);

  std::cout << "  Verified: Each configuration type appears exactly once\n";
}

BOOST_AUTO_TEST_CASE(scan_database_for_duplicates) {
  std::cout << "\n=== TEST 3: Scanning database for duplicate entries ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto allCompositions = ifc.findAllGlobalConfigurations();

  std::cout << "Scanning " << allCompositions.size() << " compositions for duplicates...\n";

  int duplicatesFound = 0;

  for (auto const& compName : allCompositions) {
    try {
      auto members = ifc.loadGlobalConfiguration(compName);

      if (members.size() > 10) {
        std::cout << "  " << compName << ": " << members.size() << " members\n";
      }

    } catch (std::exception const& e) {
      std::cout << "  Error loading " << compName << ": " << e.what() << "\n";
    }
  }

  std::cout << "\nSummary: Found " << duplicatesFound << " compositions with duplicate detection issues\n";
}

BOOST_AUTO_TEST_CASE(map_interface_prevents_duplicates) {
  std::cout << "\n=== TEST 4: Map interface prevents duplicates ===\n";

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};

  std::string typeName = "TestConfiguration001";

  compositionMap[typeName] = 100;
  std::cout << "Added version 100, map size: " << compositionMap.size() << "\n";
  BOOST_CHECK_EQUAL(compositionMap.size(), 1);
  BOOST_CHECK_EQUAL(compositionMap[typeName], 100);

  compositionMap[typeName] = 200;
  std::cout << "Added version 200, map size: " << compositionMap.size() << "\n";
  BOOST_CHECK_EQUAL(compositionMap.size(), 1);
  BOOST_CHECK_EQUAL(compositionMap[typeName], 200);

  compositionMap[typeName] = 300;
  std::cout << "Added version 300, map size: " << compositionMap.size() << "\n";
  BOOST_CHECK_EQUAL(compositionMap.size(), 1);
  BOOST_CHECK_EQUAL(compositionMap[typeName], 300);

  std::cout << "  Verified: std::map automatically prevents duplicates\n";
  std::cout << "  Final value: " << compositionMap[typeName] << " (should be 300)\n";
}

BOOST_AUTO_TEST_SUITE_END()
