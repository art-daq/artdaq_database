#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (EntitySupport test)

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct EntitySupportTestData {
  EntitySupportTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== ENTITY SUPPORT TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~EntitySupportTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

EntitySupportTestData fixture;

BOOST_AUTO_TEST_SUITE(entity_support_test)

BOOST_AUTO_TEST_CASE(store_same_version_different_config_types) {
  std::cout << "\n=== TEST 1: Store same version for different config types ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = testVersion;
  cfg1->getView().fillFromJSON("{\"type\": \"config001\"}");

  auto result1 = ifc.saveActiveVersion(cfg1.get());
  BOOST_REQUIRE_EQUAL(result1, 0);
  std::cout << "Step 1: Saved TestConfiguration001 version " << testVersion << "\n";

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = testVersion;
  cfg2->getView().fillFromJSON("{\"type\": \"config002\"}");

  auto result2 = ifc.saveActiveVersion(cfg2.get());
  BOOST_REQUIRE_EQUAL(result2, 0);
  std::cout << "Step 2: Saved TestConfiguration002 version " << testVersion << "\n";

  std::shared_ptr<ConfigurationBase> read1 = std::make_shared<TestConfiguration001>();
  auto fillResult1 = ifc.fill(read1.get(), testVersion);
  BOOST_CHECK_EQUAL(fillResult1, 0);

  std::shared_ptr<ConfigurationBase> read2 = std::make_shared<TestConfiguration002>();
  auto fillResult2 = ifc.fill(read2.get(), testVersion);
  BOOST_CHECK_EQUAL(fillResult2, 0);

  bool has001 = (read1->getView()._json.find("config001") != std::string::npos);
  bool has002 = (read2->getView()._json.find("config002") != std::string::npos);

  BOOST_CHECK_MESSAGE(has001, "TestConfiguration001 should have config001 data");
  BOOST_CHECK_MESSAGE(has002, "TestConfiguration002 should have config002 data");

  std::cout << "  Verified: Same version stored independently for different config types\n";
}

BOOST_AUTO_TEST_CASE(composition_with_multiple_config_types) {
  std::cout << "\n=== TEST 2: Composition with multiple config types ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int version1 = fixture.baseVersion + 1;
  int version2 = fixture.baseVersion + 2;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  cfg1->getView().fillFromJSON("{\"subsystem\": \"detector\"}");
  auto result1 = ifc.saveActiveVersion(cfg1.get());
  BOOST_REQUIRE_EQUAL(result1, 0);
  std::cout << "Step 1: Saved TestConfiguration001 v" << version1 << "\n";

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = version2;
  cfg2->getView().fillFromJSON("{\"subsystem\": \"trigger\"}");
  auto result2 = ifc.saveActiveVersion(cfg2.get());
  BOOST_REQUIRE_EQUAL(result2, 0);
  std::cout << "Step 2: Saved TestConfiguration002 v" << version2 << "\n";

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[cfg1->getConfigurationName()] = version1;
  map[cfg2->getConfigurationName()] = version2;

  std::string compName = "MultiType_" + std::to_string(fixture.baseVersion);
  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(map, compName));
  std::cout << "Step 3: Created composition " << compName << "\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);
  std::cout << "Step 4: Loaded composition with " << loaded.size() << " members\n";

  BOOST_CHECK_EQUAL(loaded.size(), 2);
  BOOST_CHECK_EQUAL(loaded.at(cfg1->getConfigurationName()), version1);
  BOOST_CHECK_EQUAL(loaded.at(cfg2->getConfigurationName()), version2);

  std::cout << "  Verified: Composition contains both configuration types\n";
}

BOOST_AUTO_TEST_CASE(version_isolation_per_config_type) {
  std::cout << "\n=== TEST 3: Version isolation per configuration type ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int baseV = fixture.baseVersion + 10;

  for (int i = 0; i < 3; ++i) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = baseV + i;
    auto result = ifc.saveActiveVersion(cfg.get());
    BOOST_REQUIRE_EQUAL(result, 0);
  }
  std::cout << "Step 1: Saved TestConfiguration001 versions " << baseV << "-" << (baseV + 2) << "\n";

  for (int i = 0; i < 2; ++i) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration002>();
    cfg->getView().version = baseV + 100 + i;
    auto result = ifc.saveActiveVersion(cfg.get());
    BOOST_REQUIRE_EQUAL(result, 0);
  }
  std::cout << "Step 2: Saved TestConfiguration002 versions " << (baseV + 100) << "-" << (baseV + 101) << "\n";

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  auto versions1 = ifc.getVersions(cfg1.get());

  bool found1 = (versions1.find(baseV) != versions1.end());
  bool found2 = (versions1.find(baseV + 1) != versions1.end());
  bool found3 = (versions1.find(baseV + 2) != versions1.end());

  BOOST_CHECK_MESSAGE(found1, "Version " + std::to_string(baseV) + " should exist for TestConfiguration001");
  BOOST_CHECK_MESSAGE(found2, "Version " + std::to_string(baseV + 1) + " should exist for TestConfiguration001");
  BOOST_CHECK_MESSAGE(found3, "Version " + std::to_string(baseV + 2) + " should exist for TestConfiguration001");

  bool wrongVersion1 = (versions1.find(baseV + 100) != versions1.end());
  bool wrongVersion2 = (versions1.find(baseV + 101) != versions1.end());

  BOOST_CHECK_MESSAGE(!wrongVersion1, "TestConfiguration002's version should not appear in TestConfiguration001");
  BOOST_CHECK_MESSAGE(!wrongVersion2, "TestConfiguration002's version should not appear in TestConfiguration001");

  std::cout << "  Verified: Version lists are isolated per configuration type\n";
}

BOOST_AUTO_TEST_SUITE_END()
