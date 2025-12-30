#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (MergeOverwrite test)

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <string>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct MergeOverwriteTestData {
  MergeOverwriteTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== MERGE-BASED OVERWRITE TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n";
  }

  ~MergeOverwriteTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int createConfig001(DatabaseConfigurationInterface& ifc, int version) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = version;
    return ifc.saveActiveVersion(cfg.get());
  }

  int createConfig002(DatabaseConfigurationInterface& ifc, int version) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration002>();
    cfg->getView().version = version;
    return ifc.saveActiveVersion(cfg.get());
  }

  std::string config001Name() {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    return cfg->getConfigurationName();
  }

  std::string config002Name() {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration002>();
    return cfg->getConfigurationName();
  }

  int baseVersion;
};

MergeOverwriteTestData fixture;

BOOST_AUTO_TEST_SUITE(merge_overwrite_test)

BOOST_AUTO_TEST_CASE(overwrite_same_content_succeeds) {
  std::cout << "\n=== TEST 1: Overwrite same content succeeds ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion;
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version1), 0);
  std::cout << "Step 1: Created version " << version1 << "\n";

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = version1;

  std::string compName = "OverwriteSame_" + std::to_string(fixture.baseVersion);
  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(map1, compName));
  std::cout << "Step 2: Created composition '" << compName << "'\n";

  std::cout << "Step 3: Overwriting with same content (allowOverwrite=true)...\n";
  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(map1, compName, true));
  std::cout << "  Result: SUCCESS\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 1);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], version1);

  std::cout << "  Verified: Composition unchanged after overwrite\n";
}

BOOST_AUTO_TEST_CASE(overwrite_add_new_member) {
  std::cout << "\n=== TEST 2: Overwrite adding new member ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 10;
  int version2 = fixture.baseVersion + 11;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version1), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, version2), 0);
  std::cout << "Step 1: Created versions " << version1 << ", " << version2 << "\n";

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = version1;

  std::string compName = "OverwriteAdd_" + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(map1, compName);
  std::cout << "Step 2: Created composition with 1 member\n";

  auto initial = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(initial.size(), 1);

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[fixture.config001Name()] = version1;
  map2[fixture.config002Name()] = version2;

  std::cout << "Step 3: Overwriting to add new member...\n";
  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(map2, compName, true));
  std::cout << "  Result: SUCCESS\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 2);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], version1);
  BOOST_CHECK_EQUAL(loaded[fixture.config002Name()], version2);

  std::cout << "  Verified: Composition now has 2 members\n";

  auto foundComps = ifc.findCompositionsContaining(fixture.config002Name(), std::to_string(version2));
  bool found = (foundComps.find(compName) != foundComps.end());
  BOOST_CHECK_EQUAL(found, true);

  std::cout << "  Verified: Reverse lookup finds new member\n";
}

BOOST_AUTO_TEST_CASE(overwrite_remove_member) {
  std::cout << "\n=== TEST 3: Overwrite removing member ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 20;
  int version2 = fixture.baseVersion + 21;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version1), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, version2), 0);
  std::cout << "Step 1: Created versions " << version1 << ", " << version2 << "\n";

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = version1;
  map1[fixture.config002Name()] = version2;

  std::string compName = "OverwriteRemove_" + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(map1, compName);
  std::cout << "Step 2: Created composition with 2 members\n";

  auto initial = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(initial.size(), 2);

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[fixture.config001Name()] = version1;

  std::cout << "Step 3: Overwriting to remove Config002...\n";
  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(map2, compName, true));
  std::cout << "  Result: SUCCESS\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 1);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], version1);

  bool hasConfig002 = (loaded.find(fixture.config002Name()) != loaded.end());
  BOOST_CHECK_EQUAL(hasConfig002, false);

  std::cout << "  Verified: Composition now has 1 member (Config002 removed)\n";

  auto foundComps = ifc.findCompositionsContaining(fixture.config002Name(), std::to_string(version2));
  bool stillFound = (foundComps.find(compName) != foundComps.end());
  BOOST_CHECK_EQUAL(stillFound, false);

  std::cout << "  Verified: Reverse lookup no longer finds removed member\n";
}

BOOST_AUTO_TEST_CASE(overwrite_update_version) {
  std::cout << "\n=== TEST 4: Overwrite updating member version ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int versionOld = fixture.baseVersion + 30;
  int versionNew = fixture.baseVersion + 31;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, versionOld), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, versionNew), 0);
  std::cout << "Step 1: Created versions " << versionOld << " and " << versionNew << "\n";

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = versionOld;

  std::string compName = "OverwriteUpdate_" + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(map1, compName);
  std::cout << "Step 2: Created composition with version " << versionOld << "\n";

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[fixture.config001Name()] = versionNew;

  std::cout << "Step 3: Overwriting to update version to " << versionNew << "...\n";
  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(map2, compName, true));
  std::cout << "  Result: SUCCESS\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 1);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], versionNew);

  std::cout << "  Verified: Composition now has version " << versionNew << "\n";

  auto foundOld = ifc.findCompositionsContaining(fixture.config001Name(), std::to_string(versionOld));
  bool oldFound = (foundOld.find(compName) != foundOld.end());
  BOOST_CHECK_EQUAL(oldFound, false);

  std::cout << "  Verified: Reverse lookup for old version returns empty\n";

  auto foundNew = ifc.findCompositionsContaining(fixture.config001Name(), std::to_string(versionNew));
  bool newFound = (foundNew.find(compName) != foundNew.end());
  BOOST_CHECK_EQUAL(newFound, true);

  std::cout << "  Verified: Reverse lookup for new version finds composition\n";
}

BOOST_AUTO_TEST_CASE(overwrite_complete_replacement) {
  std::cout << "\n=== TEST 5: Overwrite complete replacement ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int versionA = fixture.baseVersion + 40;
  int versionB = fixture.baseVersion + 41;
  int versionC = fixture.baseVersion + 42;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, versionA), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, versionC), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, versionB), 0);
  std::cout << "Step 1: Created versions A=" << versionA << ", B=" << versionB << ", C=" << versionC << "\n";

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = versionA;

  std::string compName = "OverwriteReplace_" + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(map1, compName);
  std::cout << "Step 2: Created composition with Config001/v" << versionA << "\n";

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[fixture.config001Name()] = versionC;
  map2[fixture.config002Name()] = versionB;

  std::cout << "Step 3: Complete replacement...\n";
  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(map2, compName, true));
  std::cout << "  Result: SUCCESS\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 2);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], versionC);
  BOOST_CHECK_EQUAL(loaded[fixture.config002Name()], versionB);

  std::cout << "  Verified: Composition replaced completely\n";

  auto foundA = ifc.findCompositionsContaining(fixture.config001Name(), std::to_string(versionA));
  BOOST_CHECK_EQUAL(foundA.find(compName) != foundA.end(), false);

  std::cout << "  Verified: Old version A not in reverse lookup\n";
}

BOOST_AUTO_TEST_CASE(overwrite_without_flag_fails) {
  std::cout << "\n=== TEST 6: Overwrite without flag fails ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 50;
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version1), 0);

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = version1;

  std::string compName = "OverwriteNoFlag_" + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(map1, compName);
  std::cout << "Step 1: Created composition\n";

  std::cout << "Step 2: Attempting overwrite without flag...\n";
  BOOST_CHECK_THROW(ifc.storeGlobalConfiguration(map1, compName), std::runtime_error);
  std::cout << "  Result: Exception thrown (as expected)\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 1);

  std::cout << "  Verified: Original composition preserved\n";
}

BOOST_AUTO_TEST_CASE(empty_composition_creation_rejected) {
  std::cout << "\n=== TEST 7: Empty composition creation rejected ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto emptyMap = DatabaseConfigurationInterface::config_version_map_t{};
  std::string compName = "EmptyComp_" + std::to_string(fixture.baseVersion);

  std::cout << "Attempting to create empty composition...\n";
  BOOST_CHECK_THROW(ifc.storeGlobalConfiguration(emptyMap, compName), std::exception);
  std::cout << "  Result: Exception thrown (as expected)\n";
  std::cout << "  Verified: Empty compositions are rejected with clear error\n";
}

BOOST_AUTO_TEST_CASE(overwrite_to_empty_rejected) {
  std::cout << "\n=== TEST 8: Overwrite to empty rejected ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 70;
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version1), 0);

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = version1;

  std::string compName = "OverwriteToEmpty_" + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(map1, compName);
  std::cout << "Step 1: Created composition with 1 member\n";

  auto emptyMap = DatabaseConfigurationInterface::config_version_map_t{};

  std::cout << "Step 2: Attempting to overwrite with empty map...\n";
  BOOST_CHECK_THROW(ifc.storeGlobalConfiguration(emptyMap, compName, true), std::exception);
  std::cout << "  Result: Exception thrown (as expected)\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 1);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], version1);

  std::cout << "  Verified: Original composition preserved after failed overwrite\n";
}

BOOST_AUTO_TEST_CASE(three_versions_bug_regression) {
  std::cout << "\n=== TEST 9: Three Versions Bug regression test ===\n";
  std::cout << "This test verifies the fix for the 'Three Versions Bug'\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 80;
  int version2 = fixture.baseVersion + 81;
  int version3 = fixture.baseVersion + 82;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version1), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version2), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version3), 0);
  std::cout << "Step 1: Created versions " << version1 << ", " << version2 << ", " << version3 << "\n";

  std::string compName = "ThreeVersionsBugTest_" + std::to_string(fixture.baseVersion);

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = version1;
  ifc.storeGlobalConfiguration(map1, compName);
  std::cout << "Step 2: Created composition with version " << version1 << "\n";

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[fixture.config001Name()] = version2;
  ifc.storeGlobalConfiguration(map2, compName, true);
  std::cout << "Step 3: Overwrote with version " << version2 << "\n";

  auto map3 = DatabaseConfigurationInterface::config_version_map_t{};
  map3[fixture.config001Name()] = version3;
  ifc.storeGlobalConfiguration(map3, compName, true);
  std::cout << "Step 4: Overwrote with version " << version3 << "\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);

  BOOST_CHECK_EQUAL(loaded.size(), 1);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], version3);

  std::cout << "Step 5: Verified composition has only version " << version3 << "\n";

  auto found1 = ifc.findCompositionsContaining(fixture.config001Name(), std::to_string(version1));
  BOOST_CHECK_MESSAGE(found1.find(compName) == found1.end(), "BUG: Version " << version1 << " should NOT be found in composition!");

  auto found2 = ifc.findCompositionsContaining(fixture.config001Name(), std::to_string(version2));
  BOOST_CHECK_MESSAGE(found2.find(compName) == found2.end(), "BUG: Version " << version2 << " should NOT be found in composition!");

  auto found3 = ifc.findCompositionsContaining(fixture.config001Name(), std::to_string(version3));
  BOOST_CHECK_MESSAGE(found3.find(compName) != found3.end(), "Version " << version3 << " should be found in composition");

  std::cout << "Step 6-8: Verified reverse lookups are correct\n";
  std::cout << "  v" << version1 << " found: " << (found1.find(compName) != found1.end() ? "YES (BUG!)" : "NO (correct)") << "\n";
  std::cout << "  v" << version2 << " found: " << (found2.find(compName) != found2.end() ? "YES (BUG!)" : "NO (correct)") << "\n";
  std::cout << "  v" << version3 << " found: " << (found3.find(compName) != found3.end() ? "YES (correct)" : "NO (BUG!)") << "\n";

  std::cout << "\n  SUCCESS: Three Versions Bug fix verified!\n";
}

BOOST_AUTO_TEST_CASE(multiple_overwrites_integrity) {
  std::cout << "\n=== TEST 10: Multiple overwrites preserve integrity ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int v1 = fixture.baseVersion + 90;
  int v2 = fixture.baseVersion + 91;
  int v3 = fixture.baseVersion + 92;
  int v4 = fixture.baseVersion + 93;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, v1), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, v3), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, v2), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, v4), 0);

  std::string compName = "MultiOverwrite_" + std::to_string(fixture.baseVersion);

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = v1;
  map1[fixture.config002Name()] = v2;
  ifc.storeGlobalConfiguration(map1, compName);

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[fixture.config001Name()] = v3;
  map2[fixture.config002Name()] = v2;
  ifc.storeGlobalConfiguration(map2, compName, true);

  auto map3 = DatabaseConfigurationInterface::config_version_map_t{};
  map3[fixture.config001Name()] = v3;
  map3[fixture.config002Name()] = v4;
  ifc.storeGlobalConfiguration(map3, compName, true);

  auto map4 = DatabaseConfigurationInterface::config_version_map_t{};
  map4[fixture.config001Name()] = v3;
  ifc.storeGlobalConfiguration(map4, compName, true);

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 1);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], v3);

  auto checkReverseLookup = [&](const std::string& type, int version, bool shouldFind) {
    auto found = ifc.findCompositionsContaining(type, std::to_string(version));
    bool isFound = (found.find(compName) != found.end());
    BOOST_CHECK_EQUAL(isFound, shouldFind);
  };

  checkReverseLookup(fixture.config001Name(), v1, false);
  checkReverseLookup(fixture.config001Name(), v3, true);
  checkReverseLookup(fixture.config002Name(), v2, false);
  checkReverseLookup(fixture.config002Name(), v4, false);

  std::cout << "  Verified: Multiple overwrites maintain integrity\n";
}

BOOST_AUTO_TEST_SUITE_END()
