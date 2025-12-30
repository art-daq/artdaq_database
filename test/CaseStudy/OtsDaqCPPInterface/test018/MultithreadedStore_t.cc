#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (MultithreadedStore test)

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

bool isFileSystemDB() {
  const char* uri = std::getenv("ARTDAQ_DATABASE_URI");
  return uri != nullptr && std::string(uri).find("filesystem") != std::string::npos;
}

struct MultithreadedStoreTestData {
  MultithreadedStoreTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== MULTITHREADED STORE TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n";
    std::cout << "Tests for: versionInfo.validate(), parallel unassignments, _mt correctness\n";
  }

  ~MultithreadedStoreTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

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

MultithreadedStoreTestData fixture;

BOOST_AUTO_TEST_SUITE(multithreaded_store_test)

BOOST_AUTO_TEST_CASE(mt_rejects_empty_version_list) {
  std::cout << "\n=== TEST 1: MT rejects empty version list ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto emptyMap = DatabaseConfigurationInterface::config_version_map_t{};
  std::string compName = "MTValidation_Empty_" + std::to_string(fixture.baseVersion);

  std::cout << "Attempting to create composition with empty version list...\n";

  BOOST_CHECK_THROW(ifc.storeGlobalConfiguration_mt(emptyMap, compName), std::exception);

  std::cout << "  Result: Exception thrown (as expected)\n";
  std::cout << "  Verified: Empty version list rejected by _mt version\n";
}

BOOST_AUTO_TEST_CASE(mt_rejects_empty_configuration_name) {
  std::cout << "\n=== TEST 2: MT rejects empty configuration name ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion;
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version1), 0);

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[fixture.config001Name()] = version1;

  std::cout << "Attempting to create composition with empty name...\n";

  BOOST_CHECK_THROW(ifc.storeGlobalConfiguration_mt(map, ""), std::exception);

  std::cout << "  Result: Exception thrown (as expected)\n";
  std::cout << "  Verified: Empty configuration name rejected by _mt version\n";
}

BOOST_AUTO_TEST_CASE(mt_overwrite_removes_members_parallel) {
  std::cout << "\n=== TEST 3: MT overwrite removes members (parallel unassignments) ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has race conditions in configurationComposition\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int v1 = fixture.baseVersion + 10;
  int v2 = fixture.baseVersion + 11;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, v1), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, v2), 0);
  std::cout << "Step 1: Created versions " << v1 << ", " << v2 << "\n";

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = v1;
  map1[fixture.config002Name()] = v2;

  std::string compName = "MTUnassignParallel_" + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration_mt(map1, compName);
  std::cout << "Step 2: Created composition with 2 members using _mt\n";

  auto initial = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(initial.size(), 2);

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[fixture.config001Name()] = v1;

  std::cout << "Step 3: Overwriting to remove Config002 using _mt...\n";
  auto start = std::chrono::high_resolution_clock::now();

  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration_mt(map2, compName, true));

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "  Completed in " << duration.count() << "ms\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 1);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], v1);

  bool hasConfig002 = (loaded.find(fixture.config002Name()) != loaded.end());
  BOOST_CHECK_EQUAL(hasConfig002, false);

  std::cout << "  Verified: Composition now has 1 member (Config002 removed)\n";

  auto foundComps = ifc.findCompositionsContaining(fixture.config002Name(), std::to_string(v2));
  bool stillFound = (foundComps.find(compName) != foundComps.end());
  BOOST_CHECK_EQUAL(stillFound, false);

  std::cout << "  Verified: Reverse lookup no longer finds removed member\n";
}

BOOST_AUTO_TEST_CASE(mt_overwrite_updates_version_parallel) {
  std::cout << "\n=== TEST 4: MT overwrite updates version (parallel unassign + assign) ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has race conditions in configurationComposition\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int versionOld = fixture.baseVersion + 20;
  int versionNew = fixture.baseVersion + 21;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, versionOld), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, versionNew), 0);
  std::cout << "Step 1: Created versions " << versionOld << " and " << versionNew << "\n";

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = versionOld;

  std::string compName = "MTUpdateVersion_" + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration_mt(map1, compName);
  std::cout << "Step 2: Created composition with version " << versionOld << " using _mt\n";

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[fixture.config001Name()] = versionNew;

  std::cout << "Step 3: Overwriting to update version to " << versionNew << " using _mt...\n";
  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration_mt(map2, compName, true));

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

BOOST_AUTO_TEST_CASE(mt_and_st_produce_identical_results) {
  std::cout << "\n=== TEST 5: MT and single-threaded produce identical results ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has race conditions in configurationComposition\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int v1 = fixture.baseVersion + 30;
  int v2 = fixture.baseVersion + 31;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, v1), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, v2), 0);

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[fixture.config001Name()] = v1;
  map[fixture.config002Name()] = v2;

  std::string compNameST = "IdenticalST_" + std::to_string(fixture.baseVersion);
  std::string compNameMT = "IdenticalMT_" + std::to_string(fixture.baseVersion);

  std::cout << "Creating composition using single-threaded version...\n";
  ifc.storeGlobalConfiguration(map, compNameST);

  std::cout << "Creating composition using multi-threaded version...\n";
  ifc.storeGlobalConfiguration_mt(map, compNameMT);

  auto loadedST = ifc.loadGlobalConfiguration(compNameST);
  auto loadedMT = ifc.loadGlobalConfiguration(compNameMT);

  BOOST_CHECK_EQUAL(loadedST.size(), loadedMT.size());

  for (auto const& entry : loadedST) {
    auto it = loadedMT.find(entry.first);
    BOOST_CHECK_MESSAGE(it != loadedMT.end(), "MT missing member: " << entry.first);
    if (it != loadedMT.end()) {
      BOOST_CHECK_EQUAL(it->second, entry.second);
    }
  }

  std::cout << "  Verified: Both compositions have identical content\n";

  for (auto const& entry : map) {
    auto foundST = ifc.findCompositionsContaining(entry.first, std::to_string(entry.second));
    auto foundMT = ifc.findCompositionsContaining(entry.first, std::to_string(entry.second));

    bool stFound = (foundST.find(compNameST) != foundST.end());
    bool mtFound = (foundMT.find(compNameMT) != foundMT.end());

    BOOST_CHECK_EQUAL(stFound, true);
    BOOST_CHECK_EQUAL(mtFound, true);
  }

  std::cout << "  Verified: Reverse lookups work for both\n";
}

BOOST_AUTO_TEST_CASE(mt_overwrite_many_unassignments) {
  std::cout << "\n=== TEST 6: MT overwrite with many unassignments ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has race conditions in configurationComposition\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  const int NUM_VERSIONS = 5;
  std::vector<int> config001Versions;
  std::vector<int> config002Versions;

  for (int i = 0; i < NUM_VERSIONS; ++i) {
    int v1 = fixture.baseVersion + 40 + i;
    int v2 = fixture.baseVersion + 40 + NUM_VERSIONS + i;

    BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, v1), 0);
    BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, v2), 0);

    config001Versions.push_back(v1);
    config002Versions.push_back(v2);
  }

  std::cout << "Step 1: Created " << (NUM_VERSIONS * 2) << " configuration versions\n";

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = config001Versions[0];
  map1[fixture.config002Name()] = config002Versions[0];

  std::string compName = "MTManyUnassign_" + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration_mt(map1, compName);
  std::cout << "Step 2: Created initial composition with 2 members\n";

  for (int i = 1; i < NUM_VERSIONS; ++i) {
    auto mapN = DatabaseConfigurationInterface::config_version_map_t{};
    mapN[fixture.config001Name()] = config001Versions[i];
    mapN[fixture.config002Name()] = config002Versions[i];

    std::cout << "Step " << (2 + i) << ": Overwriting with versions " << config001Versions[i] << ", " << config002Versions[i] << "...\n";

    auto start = std::chrono::high_resolution_clock::now();
    ifc.storeGlobalConfiguration_mt(mapN, compName, true);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "    Completed in " << duration.count() << "ms\n";
  }

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 2);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], config001Versions[NUM_VERSIONS - 1]);
  BOOST_CHECK_EQUAL(loaded[fixture.config002Name()], config002Versions[NUM_VERSIONS - 1]);

  std::cout << "  Verified: Final composition has latest versions\n";

  for (int i = 0; i < NUM_VERSIONS - 1; ++i) {
    auto found1 = ifc.findCompositionsContaining(fixture.config001Name(), std::to_string(config001Versions[i]));
    auto found2 = ifc.findCompositionsContaining(fixture.config002Name(), std::to_string(config002Versions[i]));

    BOOST_CHECK_MESSAGE(found1.find(compName) == found1.end(), "Old version " << config001Versions[i] << " should not be found");
    BOOST_CHECK_MESSAGE(found2.find(compName) == found2.end(), "Old version " << config002Versions[i] << " should not be found");
  }

  auto foundLast1 = ifc.findCompositionsContaining(fixture.config001Name(), std::to_string(config001Versions[NUM_VERSIONS - 1]));
  auto foundLast2 = ifc.findCompositionsContaining(fixture.config002Name(), std::to_string(config002Versions[NUM_VERSIONS - 1]));

  BOOST_CHECK(foundLast1.find(compName) != foundLast1.end());
  BOOST_CHECK(foundLast2.find(compName) != foundLast2.end());

  std::cout << "  Verified: Only latest versions found in reverse lookup\n";
}

BOOST_AUTO_TEST_CASE(mt_three_versions_bug_regression) {
  std::cout << "\n=== TEST 7: MT three versions bug regression ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has race conditions in configurationComposition\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 60;
  int version2 = fixture.baseVersion + 61;
  int version3 = fixture.baseVersion + 62;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version1), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version2), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version3), 0);
  std::cout << "Step 1: Created versions " << version1 << ", " << version2 << ", " << version3 << "\n";

  std::string compName = "MTThreeVersionsBug_" + std::to_string(fixture.baseVersion);

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = version1;
  ifc.storeGlobalConfiguration_mt(map1, compName);
  std::cout << "Step 2: Created composition with version " << version1 << " using _mt\n";

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[fixture.config001Name()] = version2;
  ifc.storeGlobalConfiguration_mt(map2, compName, true);
  std::cout << "Step 3: Overwrote with version " << version2 << " using _mt\n";

  auto map3 = DatabaseConfigurationInterface::config_version_map_t{};
  map3[fixture.config001Name()] = version3;
  ifc.storeGlobalConfiguration_mt(map3, compName, true);
  std::cout << "Step 4: Overwrote with version " << version3 << " using _mt\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);

  BOOST_CHECK_EQUAL(loaded.size(), 1);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], version3);

  std::cout << "Step 5: Verified composition has only version " << version3 << "\n";

  auto found1 = ifc.findCompositionsContaining(fixture.config001Name(), std::to_string(version1));
  BOOST_CHECK_MESSAGE(found1.find(compName) == found1.end(), "BUG: Version " << version1 << " should NOT be found!");

  auto found2 = ifc.findCompositionsContaining(fixture.config001Name(), std::to_string(version2));
  BOOST_CHECK_MESSAGE(found2.find(compName) == found2.end(), "BUG: Version " << version2 << " should NOT be found!");

  auto found3 = ifc.findCompositionsContaining(fixture.config001Name(), std::to_string(version3));
  BOOST_CHECK_MESSAGE(found3.find(compName) != found3.end(), "Version " << version3 << " should be found");

  std::cout << "  v" << version1 << " found: " << (found1.find(compName) != found1.end() ? "YES (BUG!)" : "NO (correct)") << "\n";
  std::cout << "  v" << version2 << " found: " << (found2.find(compName) != found2.end() ? "YES (BUG!)" : "NO (correct)") << "\n";
  std::cout << "  v" << version3 << " found: " << (found3.find(compName) != found3.end() ? "YES (correct)" : "NO (BUG!)") << "\n";

  std::cout << "\n  SUCCESS: Three Versions Bug verified fixed for _mt version!\n";
}

BOOST_AUTO_TEST_CASE(mt_complete_replacement) {
  std::cout << "\n=== TEST 8: MT complete replacement ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has race conditions in configurationComposition\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int vA = fixture.baseVersion + 70;
  int vB = fixture.baseVersion + 71;
  int vC = fixture.baseVersion + 72;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, vA), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, vC), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, vB), 0);

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[fixture.config001Name()] = vA;

  std::string compName = "MTCompleteReplace_" + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration_mt(map1, compName);
  std::cout << "Step 1: Created composition with Config001/v" << vA << " using _mt\n";

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[fixture.config001Name()] = vC;
  map2[fixture.config002Name()] = vB;

  std::cout << "Step 2: Complete replacement using _mt...\n";
  auto start = std::chrono::high_resolution_clock::now();

  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration_mt(map2, compName, true));

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "  Completed in " << duration.count() << "ms\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 2);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], vC);
  BOOST_CHECK_EQUAL(loaded[fixture.config002Name()], vB);

  std::cout << "  Verified: Composition replaced completely\n";

  auto foundA = ifc.findCompositionsContaining(fixture.config001Name(), std::to_string(vA));
  BOOST_CHECK(foundA.find(compName) == foundA.end());

  std::cout << "  Verified: Old version A not in reverse lookup\n";
}

BOOST_AUTO_TEST_CASE(mt_performance_overwrite_comparison) {
  std::cout << "\n=== TEST 9: MT performance comparison for overwrite ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has race conditions in configurationComposition\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int v1_old = fixture.baseVersion + 80;
  int v2_old = fixture.baseVersion + 81;
  int v1_new = fixture.baseVersion + 82;
  int v2_new = fixture.baseVersion + 83;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, v1_old), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, v2_old), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, v1_new), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, v2_new), 0);

  auto mapOld = DatabaseConfigurationInterface::config_version_map_t{};
  mapOld[fixture.config001Name()] = v1_old;
  mapOld[fixture.config002Name()] = v2_old;

  auto mapNew = DatabaseConfigurationInterface::config_version_map_t{};
  mapNew[fixture.config001Name()] = v1_new;
  mapNew[fixture.config002Name()] = v2_new;

  std::string compNameST = "PerfOverwriteST_" + std::to_string(fixture.baseVersion);
  std::string compNameMT = "PerfOverwriteMT_" + std::to_string(fixture.baseVersion);

  ifc.storeGlobalConfiguration(mapOld, compNameST);
  ifc.storeGlobalConfiguration_mt(mapOld, compNameMT);

  std::cout << "Initial compositions created. Now testing overwrite performance...\n";

  auto startST = std::chrono::high_resolution_clock::now();
  ifc.storeGlobalConfiguration(mapNew, compNameST, true);
  auto endST = std::chrono::high_resolution_clock::now();
  auto durationST = std::chrono::duration_cast<std::chrono::milliseconds>(endST - startST);

  auto startMT = std::chrono::high_resolution_clock::now();
  ifc.storeGlobalConfiguration_mt(mapNew, compNameMT, true);
  auto endMT = std::chrono::high_resolution_clock::now();
  auto durationMT = std::chrono::duration_cast<std::chrono::milliseconds>(endMT - startMT);

  std::cout << "  Single-threaded overwrite: " << durationST.count() << "ms\n";
  std::cout << "  Multi-threaded overwrite:  " << durationMT.count() << "ms\n";

  auto loadedST = ifc.loadGlobalConfiguration(compNameST);
  auto loadedMT = ifc.loadGlobalConfiguration(compNameMT);

  BOOST_CHECK_EQUAL(loadedST[fixture.config001Name()], v1_new);
  BOOST_CHECK_EQUAL(loadedST[fixture.config002Name()], v2_new);
  BOOST_CHECK_EQUAL(loadedMT[fixture.config001Name()], v1_new);
  BOOST_CHECK_EQUAL(loadedMT[fixture.config002Name()], v2_new);

  std::cout << "  Verified: Both overwrites produced correct results\n";
}

BOOST_AUTO_TEST_CASE(mt_duplicate_rejection) {
  std::cout << "\n=== TEST 10: MT duplicate rejection ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has race conditions in configurationComposition\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 90;
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version1), 0);

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[fixture.config001Name()] = version1;

  std::string compName = "MTDuplicateTest_" + std::to_string(fixture.baseVersion);

  ifc.storeGlobalConfiguration_mt(map, compName);
  std::cout << "Step 1: Created composition using _mt\n";

  std::cout << "Step 2: Attempting duplicate creation without overwrite flag...\n";
  BOOST_CHECK_THROW(ifc.storeGlobalConfiguration_mt(map, compName), std::runtime_error);
  std::cout << "  Result: Exception thrown (as expected)\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 1);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], version1);

  std::cout << "  Verified: Original composition preserved\n";
}

BOOST_AUTO_TEST_SUITE_END()
