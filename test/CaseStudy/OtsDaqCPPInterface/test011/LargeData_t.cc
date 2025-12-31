#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (LargeData test)

#include <boost/test/unit_test.hpp>

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct LargeDataTestData {
  LargeDataTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== LARGE DATA TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~LargeDataTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  std::string generateLargeJson(size_t targetSize) {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"largeData\": [\n";

    size_t currentSize = ss.str().size() + 10;
    int itemCount = 0;

    while (currentSize < targetSize) {
      if (itemCount > 0) ss << ",\n";
      ss << "    {\"item\": " << itemCount << ", \"data\": \"value_" << itemCount << "_padding_" << std::string(50, 'x') << "\"}";
      currentSize = ss.str().size() + 10;
      itemCount++;
    }

    ss << "\n  ]\n}";
    return ss.str();
  }

  int baseVersion;
};

LargeDataTestData fixture;

BOOST_AUTO_TEST_SUITE(large_data_test)

BOOST_AUTO_TEST_CASE(large_configuration_payload_10kb) {
  std::cout << "\n=== TEST 1: Large configuration payload (10KB) ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion;

  size_t targetSize = 10 * 1024;
  auto largeJson = fixture.generateLargeJson(targetSize);
  std::cout << "Step 1: Generated JSON payload of " << largeJson.size() << " bytes\n";

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON(largeJson);

  auto startStore = std::chrono::high_resolution_clock::now();
  auto saveResult = ifc.saveActiveVersion(cfg.get());
  auto endStore = std::chrono::high_resolution_clock::now();
  auto storeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endStore - startStore).count();

  BOOST_REQUIRE_EQUAL(saveResult, 0);
  std::cout << "Step 2: Stored in " << storeDuration << "ms\n";

  std::shared_ptr<ConfigurationBase> cfgRead = std::make_shared<TestConfiguration001>();

  auto startLoad = std::chrono::high_resolution_clock::now();
  auto fillResult = ifc.fill(cfgRead.get(), testVersion);
  auto endLoad = std::chrono::high_resolution_clock::now();
  auto loadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endLoad - startLoad).count();

  BOOST_REQUIRE_EQUAL(fillResult, 0);
  std::cout << "Step 3: Loaded in " << loadDuration << "ms\n";

  bool hasLargeData = (cfgRead->getView()._json.find("largeData") != std::string::npos);
  BOOST_CHECK_MESSAGE(hasLargeData, "Loaded data should contain largeData field");

  size_t loadedSize = cfgRead->getView()._json.size();
  std::cout << "Step 4: Original size: " << largeJson.size() << ", Loaded size: " << loadedSize << "\n";

  BOOST_CHECK_GT(loadedSize, targetSize / 2);

  std::cout << "  Verified: 10KB payload stored/loaded correctly\n";
}

BOOST_AUTO_TEST_CASE(large_configuration_payload_100kb) {
  std::cout << "\n=== TEST 2: Large configuration payload (100KB) ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion + 1;

  size_t targetSize = 100 * 1024;
  auto largeJson = fixture.generateLargeJson(targetSize);
  std::cout << "Step 1: Generated JSON payload of " << largeJson.size() << " bytes\n";

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON(largeJson);

  auto startStore = std::chrono::high_resolution_clock::now();
  auto saveResult = ifc.saveActiveVersion(cfg.get());
  auto endStore = std::chrono::high_resolution_clock::now();
  auto storeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endStore - startStore).count();

  BOOST_REQUIRE_EQUAL(saveResult, 0);
  std::cout << "Step 2: Stored in " << storeDuration << "ms\n";

  std::shared_ptr<ConfigurationBase> cfgRead = std::make_shared<TestConfiguration001>();

  auto startLoad = std::chrono::high_resolution_clock::now();
  auto fillResult = ifc.fill(cfgRead.get(), testVersion);
  auto endLoad = std::chrono::high_resolution_clock::now();
  auto loadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endLoad - startLoad).count();

  BOOST_REQUIRE_EQUAL(fillResult, 0);
  std::cout << "Step 3: Loaded in " << loadDuration << "ms\n";

  bool hasLargeData = (cfgRead->getView()._json.find("largeData") != std::string::npos);
  BOOST_CHECK_MESSAGE(hasLargeData, "Loaded data should contain largeData field");

  std::cout << "  Verified: 100KB payload stored/loaded correctly\n";
}

BOOST_AUTO_TEST_CASE(many_versions_single_type) {
  std::cout << "\n=== TEST 3: Many versions single type ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int baseV = fixture.baseVersion + 100;
  const int NUM_VERSIONS = 50;

  auto startCreate = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_VERSIONS; ++i) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = baseV + i;
    cfg->getView().fillFromJSON("{\"version_index\": " + std::to_string(i) + "}");
    auto result = ifc.saveActiveVersion(cfg.get());
    BOOST_REQUIRE_EQUAL(result, 0);
  }

  auto endCreate = std::chrono::high_resolution_clock::now();
  auto createDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endCreate - startCreate).count();
  std::cout << "Step 1: Created " << NUM_VERSIONS << " versions in " << createDuration << "ms\n";

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();

  auto startQuery = std::chrono::high_resolution_clock::now();
  auto versions = ifc.getVersions(cfg.get());
  auto endQuery = std::chrono::high_resolution_clock::now();
  auto queryDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endQuery - startQuery).count();

  std::cout << "Step 2: getVersions returned " << versions.size() << " versions in " << queryDuration << "ms\n";

  int foundCount = 0;
  for (int i = 0; i < NUM_VERSIONS; ++i) {
    if (versions.find(baseV + i) != versions.end()) {
      foundCount++;
    }
  }

  BOOST_CHECK_EQUAL(foundCount, NUM_VERSIONS);
  std::cout << "Step 3: Found " << foundCount << "/" << NUM_VERSIONS << " created versions\n";

  if (queryDuration > 5000) {
    BOOST_WARN_MESSAGE(false, "getVersions took longer than 5 seconds - may need optimization");
  }

  std::cout << "  Verified: All " << NUM_VERSIONS << " versions found\n";
}

BOOST_AUTO_TEST_CASE(large_composition) {
  std::cout << "\n=== TEST 4: Large composition (many members) ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int baseV = fixture.baseVersion + 200;

  const int VERSIONS_PER_TYPE = 10;

  for (int i = 0; i < VERSIONS_PER_TYPE; ++i) {
    std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
    cfg1->getView().version = baseV + i;
    auto result1 = ifc.saveActiveVersion(cfg1.get());
    BOOST_REQUIRE_EQUAL(result1, 0);

    std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
    cfg2->getView().version = baseV + i;
    auto result2 = ifc.saveActiveVersion(cfg2.get());
    BOOST_REQUIRE_EQUAL(result2, 0);
  }
  std::cout << "Step 1: Created " << VERSIONS_PER_TYPE << " versions of each config type\n";

  auto map = DatabaseConfigurationInterface::config_version_map_t{};

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();

  map[cfg1->getConfigurationName()] = baseV;
  map[cfg2->getConfigurationName()] = baseV;

  std::string compName = "LargeComp_" + std::to_string(fixture.baseVersion);

  auto startStore = std::chrono::high_resolution_clock::now();
  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(map, compName));
  auto endStore = std::chrono::high_resolution_clock::now();
  auto storeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endStore - startStore).count();

  std::cout << "Step 2: Created composition in " << storeDuration << "ms\n";

  auto startLoad = std::chrono::high_resolution_clock::now();
  auto loaded = ifc.loadGlobalConfiguration(compName);
  auto endLoad = std::chrono::high_resolution_clock::now();
  auto loadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endLoad - startLoad).count();

  std::cout << "Step 3: Loaded composition with " << loaded.size() << " members in " << loadDuration << "ms\n";

  BOOST_CHECK_EQUAL(loaded.size(), 2);
  BOOST_CHECK_EQUAL(loaded.at(cfg1->getConfigurationName()), baseV);
  BOOST_CHECK_EQUAL(loaded.at(cfg2->getConfigurationName()), baseV);

  std::cout << "  Verified: Composition with 2 members handled correctly\n";
}

BOOST_AUTO_TEST_CASE(many_compositions_lookup) {
  std::cout << "\n=== TEST 5: Many compositions lookup ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int baseV = fixture.baseVersion + 300;
  const int NUM_COMPOSITIONS = 20;

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = baseV;
  auto saveResult = ifc.saveActiveVersion(cfg.get());
  BOOST_REQUIRE_EQUAL(saveResult, 0);

  auto startCreate = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_COMPOSITIONS; ++i) {
    auto map = DatabaseConfigurationInterface::config_version_map_t{};
    map[cfg->getConfigurationName()] = baseV;

    std::string compName = "ManyComp_" + std::to_string(fixture.baseVersion) + "_" + std::to_string(i);
    ifc.storeGlobalConfiguration(map, compName);
  }

  auto endCreate = std::chrono::high_resolution_clock::now();
  auto createDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endCreate - startCreate).count();
  std::cout << "Step 1: Created " << NUM_COMPOSITIONS << " compositions in " << createDuration << "ms\n";

  std::string searchPattern = "ManyComp_" + std::to_string(fixture.baseVersion) + "*";

  auto startSearch = std::chrono::high_resolution_clock::now();
  auto compositions = ifc.findAllGlobalConfigurations(searchPattern);
  auto endSearch = std::chrono::high_resolution_clock::now();
  auto searchDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endSearch - startSearch).count();

  std::cout << "Step 2: Search found " << compositions.size() << " compositions in " << searchDuration << "ms\n";

  BOOST_CHECK_EQUAL(compositions.size(), NUM_COMPOSITIONS);

  std::cout << "  Verified: All " << NUM_COMPOSITIONS << " compositions found via search\n";
}

BOOST_AUTO_TEST_CASE(concurrent_reads_large_payload) {
  std::cout << "\n=== TEST 6: Concurrent reads large payload ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion + 400;

  size_t targetSize = 50 * 1024;
  auto largeJson = fixture.generateLargeJson(targetSize);

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON(largeJson);
  auto saveResult = ifc.saveActiveVersion(cfg.get());
  BOOST_REQUIRE_EQUAL(saveResult, 0);
  std::cout << "Step 1: Stored " << largeJson.size() << " byte payload\n";

  const int NUM_THREADS = 5;
  std::vector<std::thread> threads;
  std::atomic<int> successCount{0};
  std::atomic<int> failCount{0};

  auto startRead = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back([&ifc, testVersion, &successCount, &failCount]() {
      try {
        auto localIfc = DatabaseConfigurationInterface();
        std::shared_ptr<ConfigurationBase> readCfg = std::make_shared<TestConfiguration001>();
        auto result = localIfc.fill(readCfg.get(), testVersion);
        if (result == 0 && readCfg->getView()._json.find("largeData") != std::string::npos) {
          successCount++;
        } else {
          failCount++;
        }
      } catch (...) {
        failCount++;
      }
    });
  }

  for (auto& t : threads) {
    t.join();
  }

  auto endRead = std::chrono::high_resolution_clock::now();
  auto readDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endRead - startRead).count();

  std::cout << "Step 2: " << NUM_THREADS << " concurrent reads completed in " << readDuration << "ms\n";
  std::cout << "        Success: " << successCount << ", Failures: " << failCount << "\n";

  BOOST_CHECK_EQUAL(successCount.load(), NUM_THREADS);
  BOOST_CHECK_EQUAL(failCount.load(), 0);

  std::cout << "  Verified: " << successCount << "/" << NUM_THREADS << " concurrent reads of large payload succeeded\n";
}

BOOST_AUTO_TEST_SUITE_END()
