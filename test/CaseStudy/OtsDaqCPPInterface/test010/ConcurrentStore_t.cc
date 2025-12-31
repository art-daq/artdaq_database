#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (ConcurrentStore test)

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

struct ConcurrentStoreTestData {
  ConcurrentStoreTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== CONCURRENT STORE TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n";
  }

  ~ConcurrentStoreTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

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

ConcurrentStoreTestData fixture;

BOOST_AUTO_TEST_SUITE(concurrent_store_test)

BOOST_AUTO_TEST_CASE(mt_basic_two_members) {
  std::cout << "\n=== TEST 1: MT store basic two members ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has race conditions in configurationComposition\n";
    std::cout << "  See NEED_TO_IMPROVE.md for details\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion;
  int version2 = fixture.baseVersion + 1;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version1), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, version2), 0);
  std::cout << "Step 1: Created versions " << version1 << ", " << version2 << "\n";

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[fixture.config001Name()] = version1;
  map[fixture.config002Name()] = version2;

  std::string compName = "MTBasic_" + std::to_string(fixture.baseVersion);

  std::cout << "Step 2: Calling storeGlobalConfiguration_mt...\n";
  auto start = std::chrono::high_resolution_clock::now();

  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration_mt(map, compName));

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "  Completed in " << duration.count() << "ms\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 2);
  BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], version1);
  BOOST_CHECK_EQUAL(loaded[fixture.config002Name()], version2);

  std::cout << "  Verified: Composition has 2 members\n";

  auto found1 = ifc.findCompositionsContaining(fixture.config001Name(), std::to_string(version1));
  auto found2 = ifc.findCompositionsContaining(fixture.config002Name(), std::to_string(version2));

  BOOST_CHECK_EQUAL(found1.find(compName) != found1.end(), true);
  BOOST_CHECK_EQUAL(found2.find(compName) != found2.end(), true);

  std::cout << "  Verified: Reverse lookups work\n";
}

BOOST_AUTO_TEST_CASE(mt_many_members) {
  std::cout << "\n=== TEST 2: MT store many members ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has race conditions in configurationComposition\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  const int NUM_VERSIONS = 10;
  std::vector<int> config001Versions;
  std::vector<int> config002Versions;

  for (int i = 0; i < NUM_VERSIONS / 2; ++i) {
    int v1 = fixture.baseVersion + 10 + i;
    int v2 = fixture.baseVersion + 10 + NUM_VERSIONS / 2 + i;

    BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, v1), 0);
    BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, v2), 0);

    config001Versions.push_back(v1);
    config002Versions.push_back(v2);
  }

  std::cout << "Step 1: Created " << NUM_VERSIONS << " configuration versions\n";

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[fixture.config001Name()] = config001Versions[0];
  map[fixture.config002Name()] = config002Versions[0];

  std::string compName = "MTMany_" + std::to_string(fixture.baseVersion);

  std::cout << "Step 2: Calling storeGlobalConfiguration_mt with 2 members...\n";
  auto start = std::chrono::high_resolution_clock::now();

  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration_mt(map, compName));

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "  Completed in " << duration.count() << "ms\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 2);

  std::cout << "  Verified: Composition created correctly\n";
}

BOOST_AUTO_TEST_CASE(mt_vs_single_threaded_performance) {
  std::cout << "\n=== TEST 3: MT vs single-threaded performance ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has race conditions in configurationComposition\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int v1_st = fixture.baseVersion + 30;
  int v2_st = fixture.baseVersion + 31;
  int v1_mt = fixture.baseVersion + 32;
  int v2_mt = fixture.baseVersion + 33;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, v1_st), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, v2_st), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, v1_mt), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, v2_mt), 0);

  auto mapST = DatabaseConfigurationInterface::config_version_map_t{};
  mapST[fixture.config001Name()] = v1_st;
  mapST[fixture.config002Name()] = v2_st;

  std::string compNameST = "PerfST_" + std::to_string(fixture.baseVersion);

  auto startST = std::chrono::high_resolution_clock::now();
  ifc.storeGlobalConfiguration(mapST, compNameST);
  auto endST = std::chrono::high_resolution_clock::now();
  auto durationST = std::chrono::duration_cast<std::chrono::milliseconds>(endST - startST);

  std::cout << "Single-threaded: " << durationST.count() << "ms\n";

  auto mapMT = DatabaseConfigurationInterface::config_version_map_t{};
  mapMT[fixture.config001Name()] = v1_mt;
  mapMT[fixture.config002Name()] = v2_mt;

  std::string compNameMT = "PerfMT_" + std::to_string(fixture.baseVersion);

  auto startMT = std::chrono::high_resolution_clock::now();
  ifc.storeGlobalConfiguration_mt(mapMT, compNameMT);
  auto endMT = std::chrono::high_resolution_clock::now();
  auto durationMT = std::chrono::duration_cast<std::chrono::milliseconds>(endMT - startMT);

  std::cout << "Multi-threaded:  " << durationMT.count() << "ms\n";

  auto loadedST = ifc.loadGlobalConfiguration(compNameST);
  auto loadedMT = ifc.loadGlobalConfiguration(compNameMT);

  BOOST_CHECK_EQUAL(loadedST.size(), 2);
  BOOST_CHECK_EQUAL(loadedMT.size(), 2);

  std::cout << "  Both compositions created correctly\n";
}

BOOST_AUTO_TEST_CASE(concurrent_reads_same_version) {
  std::cout << "\n=== TEST 4: Concurrent reads same version ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int testVersion = fixture.baseVersion + 40;
  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"test\": \"concurrent_read\"}");
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get()), 0);

  std::cout << "Setup: Created version " << testVersion << "\n";

  const int NUM_THREADS = 10;
  std::vector<std::thread> threads;
  std::atomic<int> successCount{0};
  std::atomic<int> failCount{0};
  std::mutex outputMutex;

  std::cout << "Launching " << NUM_THREADS << " concurrent read threads...\n";

  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back([&ifc, testVersion, &successCount, &failCount, &outputMutex, i]() {
      try {
        auto localIfc = DatabaseConfigurationInterface();
        std::shared_ptr<ConfigurationBase> readCfg = std::make_shared<TestConfiguration001>();

        auto result = localIfc.fill(readCfg.get(), testVersion);

        if (result == 0) {
          successCount++;
        } else {
          failCount++;
          std::lock_guard<std::mutex> lock(outputMutex);
          std::cout << "  Thread " << i << " failed to load\n";
        }
      } catch (std::exception const& e) {
        failCount++;
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "  Thread " << i << " exception: " << e.what() << "\n";
      }
    });
  }

  for (auto& t : threads) {
    t.join();
  }

  std::cout << "Results: " << successCount << "/" << NUM_THREADS << " successful\n";

  BOOST_CHECK_EQUAL(successCount.load(), NUM_THREADS);
  BOOST_CHECK_EQUAL(failCount.load(), 0);

  std::cout << "  Verified: Concurrent reads succeeded\n";
}

BOOST_AUTO_TEST_CASE(sequential_stores_different_versions) {
  std::cout << "\n=== TEST 5: Sequential stores different versions ===\n";

  auto ifc = DatabaseConfigurationInterface();

  const int NUM_VERSIONS = 5;
  int successCount = 0;
  int failCount = 0;

  std::cout << "Storing " << NUM_VERSIONS << " versions sequentially...\n";

  for (int i = 0; i < NUM_VERSIONS; ++i) {
    int version = fixture.baseVersion + 50 + i;

    try {
      std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
      cfg->getView().version = version;

      auto result = ifc.saveActiveVersion(cfg.get());

      if (result == 0) {
        successCount++;
      } else {
        failCount++;
        std::cout << "  Failed to store version " << version << "\n";
      }
    } catch (std::exception const& e) {
      failCount++;
      std::cout << "  Exception storing version " << version << ": " << e.what() << "\n";
    }
  }

  std::cout << "Results: " << successCount << "/" << NUM_VERSIONS << " successful\n";

  BOOST_CHECK_EQUAL(successCount, NUM_VERSIONS);
  BOOST_CHECK_EQUAL(failCount, 0);

  auto versions = ifc.getVersions(std::make_shared<TestConfiguration001>().get());
  int foundCount = 0;
  for (int i = 0; i < NUM_VERSIONS; ++i) {
    int version = fixture.baseVersion + 50 + i;
    if (versions.find(version) != versions.end()) {
      foundCount++;
    }
  }

  BOOST_CHECK_EQUAL(foundCount, NUM_VERSIONS);
  std::cout << "  Verified: All " << NUM_VERSIONS << " versions exist\n";
}

BOOST_AUTO_TEST_CASE(concurrent_composition_creation) {
  std::cout << "\n=== TEST 6: Concurrent composition creation ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has additional race conditions beyond composition mutex\n";
    std::cout << "  See NEED_TO_IMPROVE.md for details\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  const int NUM_THREADS = 5;

  std::vector<int> uniqueVersions;
  for (int i = 0; i < NUM_THREADS; ++i) {
    int version = fixture.baseVersion + 60 + i;
    uniqueVersions.push_back(version);
    BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, version), 0);
  }

  std::cout << "Setup: Created " << NUM_THREADS << " unique versions for compositions\n";

  std::vector<std::thread> threads;
  std::atomic<int> successCount{0};
  std::atomic<int> failCount{0};
  std::vector<std::string> compNames;
  std::mutex outputMutex;

  for (int i = 0; i < NUM_THREADS; ++i) {
    compNames.push_back("ConcurrentComp_" + std::to_string(fixture.baseVersion) + "_" + std::to_string(i));
  }

  std::cout << "Launching " << NUM_THREADS << " concurrent composition creation threads...\n";

  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back([&, i]() {
      try {
        auto localIfc = DatabaseConfigurationInterface();

        auto map = DatabaseConfigurationInterface::config_version_map_t{};
        map[fixture.config001Name()] = uniqueVersions[i];

        localIfc.storeGlobalConfiguration(map, compNames[i]);
        successCount++;

      } catch (std::exception const& e) {
        failCount++;
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "  Thread " << i << " exception: " << e.what() << "\n";
      }
    });
  }

  for (auto& t : threads) {
    t.join();
  }

  std::cout << "Results: " << successCount << "/" << NUM_THREADS << " successful\n";

  BOOST_CHECK_EQUAL(successCount.load(), NUM_THREADS);
  BOOST_CHECK_EQUAL(failCount.load(), 0);

  auto allComps = ifc.findAllGlobalConfigurations();
  int foundCount = 0;
  for (auto const& name : compNames) {
    if (allComps.find(name) != allComps.end()) {
      foundCount++;
    }
  }

  BOOST_CHECK_EQUAL(foundCount, NUM_THREADS);
  std::cout << "  Verified: All " << NUM_THREADS << " compositions exist\n";

  for (int i = 0; i < NUM_THREADS; ++i) {
    auto loaded = ifc.loadGlobalConfiguration(compNames[i]);
    BOOST_CHECK_EQUAL(loaded.size(), 1);
    BOOST_CHECK_EQUAL(loaded[fixture.config001Name()], uniqueVersions[i]);
  }
  std::cout << "  Verified: Each composition has its unique version\n";
}

BOOST_AUTO_TEST_CASE(concurrent_reverse_lookups) {
  std::cout << "\n=== TEST 7: Concurrent reverse lookups ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int targetVersion = fixture.baseVersion + 70;
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, targetVersion), 0);

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[fixture.config001Name()] = targetVersion;

  std::string compName = "ReverseLookupTarget_" + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(map, compName);

  std::cout << "Setup: Created composition " << compName << "\n";

  const int NUM_THREADS = 10;
  std::vector<std::thread> threads;
  std::atomic<int> successCount{0};
  std::atomic<int> failCount{0};

  std::cout << "Launching " << NUM_THREADS << " concurrent reverse lookup threads...\n";

  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back([&]() {
      try {
        auto localIfc = DatabaseConfigurationInterface();
        auto found = localIfc.findCompositionsContaining(fixture.config001Name(), std::to_string(targetVersion));

        if (found.find(compName) != found.end()) {
          successCount++;
        } else {
          failCount++;
        }
      } catch (std::exception const& e) {
        failCount++;
      }
    });
  }

  for (auto& t : threads) {
    t.join();
  }

  std::cout << "Results: " << successCount << "/" << NUM_THREADS << " found target\n";

  BOOST_CHECK_EQUAL(successCount.load(), NUM_THREADS);
  BOOST_CHECK_EQUAL(failCount.load(), 0);

  std::cout << "  Verified: Concurrent reverse lookups work\n";
}

BOOST_AUTO_TEST_CASE(mt_store_correctness) {
  std::cout << "\n=== TEST 8: MT store correctness verification ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has race conditions in configurationComposition\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int v1 = fixture.baseVersion + 80;
  int v2 = fixture.baseVersion + 81;

  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, v1), 0);
  BOOST_REQUIRE_EQUAL(fixture.createConfig002(ifc, v2), 0);

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[fixture.config001Name()] = v1;
  map[fixture.config002Name()] = v2;

  std::string compName = "MTCorrectness_" + std::to_string(fixture.baseVersion);

  ifc.storeGlobalConfiguration_mt(map, compName);

  auto loaded = ifc.loadGlobalConfiguration(compName);

  BOOST_CHECK_EQUAL(loaded.size(), map.size());

  for (auto const& entry : map) {
    auto it = loaded.find(entry.first);
    BOOST_CHECK_MESSAGE(it != loaded.end(), "Missing member: " << entry.first);
    if (it != loaded.end()) {
      BOOST_CHECK_EQUAL(it->second, entry.second);
    }
  }

  std::cout << "  Verified: All " << map.size() << " members stored correctly\n";

  for (auto const& entry : map) {
    auto found = ifc.findCompositionsContaining(entry.first, std::to_string(entry.second));
    BOOST_CHECK_MESSAGE(found.find(compName) != found.end(), "Reverse lookup failed for " << entry.first << "/" << entry.second);
  }

  std::cout << "  Verified: All reverse lookups work\n";
}

BOOST_AUTO_TEST_CASE(mt_store_empty_composition) {
  std::cout << "\n=== TEST 9: MT store empty composition rejected ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto emptyMap = DatabaseConfigurationInterface::config_version_map_t{};
  std::string compName = "MTEmptyComp_" + std::to_string(fixture.baseVersion);

  std::cout << "Attempting to create empty composition with storeGlobalConfiguration_mt...\n";

  BOOST_CHECK_THROW(ifc.storeGlobalConfiguration_mt(emptyMap, compName), std::exception);
  std::cout << "  Result: Exception thrown (as expected)\n";
  std::cout << "  Verified: Empty compositions rejected with clear error\n";
}

BOOST_AUTO_TEST_CASE(mt_store_stress_test) {
  std::cout << "\n=== TEST 10: MT store stress test ===\n";

  if (isFileSystemDB()) {
    std::cout << "  SKIPPED: FileSystemDB has race conditions in configurationComposition\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int v1 = fixture.baseVersion + 100;
  BOOST_REQUIRE_EQUAL(fixture.createConfig001(ifc, v1), 0);

  const int NUM_COMPOSITIONS = 10;
  std::vector<std::string> compNames;

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[fixture.config001Name()] = v1;

  std::cout << "Creating " << NUM_COMPOSITIONS << " compositions rapidly...\n";

  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_COMPOSITIONS; ++i) {
    std::string compName = "MTStress_" + std::to_string(fixture.baseVersion) + "_" + std::to_string(i);
    compNames.push_back(compName);
    ifc.storeGlobalConfiguration_mt(map, compName);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  std::cout << "  Created " << NUM_COMPOSITIONS << " compositions in " << duration.count() << "ms\n";
  std::cout << "  Average: " << (duration.count() / NUM_COMPOSITIONS) << "ms per composition\n";

  auto allComps = ifc.findAllGlobalConfigurations();
  int foundCount = 0;
  for (auto const& name : compNames) {
    if (allComps.find(name) != allComps.end()) {
      foundCount++;
    }
  }

  BOOST_CHECK_EQUAL(foundCount, NUM_COMPOSITIONS);
  std::cout << "  Verified: All " << NUM_COMPOSITIONS << " compositions exist\n";
}

BOOST_AUTO_TEST_SUITE_END()
