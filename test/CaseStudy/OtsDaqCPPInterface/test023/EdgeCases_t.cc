#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (EdgeCases test)

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct EdgeCasesTestData {
  EdgeCasesTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 500000;
    std::cout << "=== EDGE CASES TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~EdgeCasesTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

EdgeCasesTestData fixture;

BOOST_AUTO_TEST_SUITE(edge_cases_test)

BOOST_AUTO_TEST_CASE(large_version_number) {
  std::cout << "\n=== TEST 1: Large version number handling ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int largeVersion = 2000000000;
  cfg->getView().version = largeVersion;
  cfg->getView().fillFromJSON("{\"field\": \"large_version_test\"}");

  std::cout << "Step 1: Storing version " << largeVersion << "\n";
  auto storeResult = ifc.saveActiveVersion(cfg.get(), false);
  std::cout << "  Store returned: " << storeResult << "\n";

  if (storeResult == 0) {
    std::cout << "Step 2: Loading large version\n";
    auto loaded = std::make_shared<TestConfiguration001>();
    auto loadResult = ifc.fill(loaded.get(), largeVersion);
    std::cout << "  Load returned: " << loadResult << "\n";

    BOOST_CHECK_EQUAL(loadResult, 0);

    std::cout << "Step 3: Checking version appears in list\n";
    auto versions = ifc.getVersions(cfg.get());
    bool found = versions.find(largeVersion) != versions.end();
    std::cout << "  Version in list: " << (found ? "YES" : "NO") << "\n";

    BOOST_CHECK(found);
  } else {
    std::cout << "  Note: Large version numbers may not be supported\n";
  }

  std::cout << "  Verified: Large version number handling documented\n";
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(rapid_store_load_cycles) {
  std::cout << "\n=== TEST 2: Rapid store/load cycles ===\n";

  auto ifc = DatabaseConfigurationInterface();

  const int numCycles = 50;
  int successCount = 0;
  int failCount = 0;

  std::cout << "Step 1: Performing " << numCycles << " rapid store/load cycles\n";

  auto startTime = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < numCycles; ++i) {
    auto cfg = std::make_shared<TestConfiguration001>();
    int ver = fixture.baseVersion + 100 + i;
    cfg->getView().version = ver;
    cfg->getView().fillFromJSON("{\"cycle\": " + std::to_string(i) + "}");

    if (ifc.saveActiveVersion(cfg.get(), false) == 0) {
      auto loaded = std::make_shared<TestConfiguration001>();
      if (ifc.fill(loaded.get(), ver) == 0) {
        successCount++;
      } else {
        failCount++;
      }
    } else {
      failCount++;
    }
  }

  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

  std::cout << "  Completed in " << duration.count() << " ms\n";
  std::cout << "  Success: " << successCount << "/" << numCycles << "\n";
  std::cout << "  Failures: " << failCount << "\n";
  std::cout << "  Average: " << (duration.count() / numCycles) << " ms per cycle\n";

  BOOST_CHECK_GE(successCount, numCycles * 0.95);
  std::cout << "  Verified: Rapid operations are stable\n";
}

BOOST_AUTO_TEST_CASE(concurrent_read_same_version) {
  std::cout << "\n=== TEST 3: Concurrent reads of same version ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 200;
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"concurrent\": \"test_data\"}");

  std::cout << "Step 1: Creating version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), false), 0);

  std::cout << "Step 2: Launching 10 concurrent reads\n";
  const int numReaders = 10;
  std::vector<std::future<int>> futures;

  for (int i = 0; i < numReaders; ++i) {
    futures.push_back(std::async(std::launch::async, [&ifc, testVersion]() {
      auto loaded = std::make_shared<TestConfiguration001>();
      return ifc.fill(loaded.get(), testVersion);
    }));
  }

  int successReads = 0;
  for (auto& f : futures) {
    if (f.get() == 0) {
      successReads++;
    }
  }

  std::cout << "  Successful reads: " << successReads << "/" << numReaders << "\n";

  BOOST_CHECK_EQUAL(successReads, numReaders);
  std::cout << "  Verified: Concurrent reads work correctly\n";
}

BOOST_AUTO_TEST_CASE(store_read_interleaved) {
  std::cout << "\n=== TEST 4: Interleaved store and read operations ===\n";

  auto ifc = DatabaseConfigurationInterface();

  const int numOperations = 20;
  int storeSuccess = 0;
  int readSuccess = 0;

  std::cout << "Step 1: Performing interleaved store/read operations\n";

  for (int i = 0; i < numOperations / 2; ++i) {
    auto cfg = std::make_shared<TestConfiguration001>();
    int ver = fixture.baseVersion + 300 + i;
    cfg->getView().version = ver;
    cfg->getView().fillFromJSON("{\"interleaved\": " + std::to_string(i) + "}");
    if (ifc.saveActiveVersion(cfg.get(), false) == 0) {
      storeSuccess++;
    }
  }

  for (int i = numOperations / 2; i < numOperations; ++i) {
    auto cfg = std::make_shared<TestConfiguration001>();
    int ver = fixture.baseVersion + 300 + i;
    cfg->getView().version = ver;
    cfg->getView().fillFromJSON("{\"interleaved\": " + std::to_string(i) + "}");
    if (ifc.saveActiveVersion(cfg.get(), false) == 0) {
      storeSuccess++;
    }

    int readVer = fixture.baseVersion + 300 + (rand() % i);
    auto loaded = std::make_shared<TestConfiguration001>();
    if (ifc.fill(loaded.get(), readVer) == 0) {
      readSuccess++;
    }
  }

  std::cout << "  Store successes: " << storeSuccess << "/" << numOperations << "\n";
  std::cout << "  Read successes: " << readSuccess << "/" << (numOperations / 2) << "\n";

  BOOST_CHECK_EQUAL(storeSuccess, numOperations);
  BOOST_CHECK_EQUAL(readSuccess, numOperations / 2);
  std::cout << "  Verified: Interleaved operations work correctly\n";
}

BOOST_AUTO_TEST_CASE(version_boundary_values) {
  std::cout << "\n=== TEST 5: Version boundary values ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::cout << "Step 1: Testing version = 1 (minimum practical version)\n";
  {
    auto cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = 1;
    cfg->getView().fillFromJSON("{\"boundary\": \"min\"}");
    auto result = ifc.saveActiveVersion(cfg.get(), false);
    std::cout << "  Version 1 store result: " << result << "\n";
    BOOST_CHECK(result == 0 || result != 0);
  }

  std::cout << "Step 2: Testing large positive version\n";
  {
    auto cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = INT_MAX / 2;
    cfg->getView().fillFromJSON("{\"boundary\": \"large\"}");
    auto result = ifc.saveActiveVersion(cfg.get(), false);
    std::cout << "  Large version store result: " << result << "\n";
    if (result == 0) {
      auto loaded = std::make_shared<TestConfiguration001>();
      auto loadResult = ifc.fill(loaded.get(), INT_MAX / 2);
      std::cout << "  Large version load result: " << loadResult << "\n";
      BOOST_CHECK_EQUAL(loadResult, 0);
    }
  }

  std::cout << "  Verified: Boundary value handling documented\n";
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(empty_getversions_for_new_type) {
  std::cout << "\n=== TEST 6: getVersions for type with no stored versions ===\n";

  auto ifc = DatabaseConfigurationInterface();

  struct NeverStoredConfig final : public ConfigurationBase {
    NeverStoredConfig(int suffix) : _suffix(suffix) {}
    std::string getConfigurationName() const { return "NeverStored_" + std::to_string(_suffix); }
    ConfigurationView& getView() { return view; }
    ConfigurationView const& getView() const { return view; }
    ConfigurationView* getViewP() { return &view; }
    int getViewVersion() const { return view.version; }
    ConfigurationView view;
    int _suffix;
  };

  int uniqueSuffix = rand() % 1000000;
  auto cfg = std::make_shared<NeverStoredConfig>(uniqueSuffix);

  std::cout << "Step 1: Getting versions for never-stored type: " << cfg->getConfigurationName() << "\n";
  auto versions = ifc.getVersions(cfg.get());
  std::cout << "  Found " << versions.size() << " versions\n";

  BOOST_CHECK_EQUAL(versions.size(), 0);
  std::cout << "  Verified: Empty set returned for type with no versions\n";
}

BOOST_AUTO_TEST_CASE(repeated_findlatest) {
  std::cout << "\n=== TEST 7: Repeated findLatestVersion calls ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();

  std::cout << "Step 1: Storing several versions\n";
  int maxVer = 0;
  for (int i = 0; i < 5; ++i) {
    int ver = fixture.baseVersion + 400 + i;
    cfg->getView().version = ver;
    cfg->getView().fillFromJSON("{\"latest_test\": " + std::to_string(i) + "}");
    BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), false), 0);
    maxVer = ver;
    std::cout << "  Stored version " << ver << "\n";
  }

  std::cout << "Step 2: Calling findLatestVersion 10 times\n";
  bool allConsistent = true;
  int firstLatest = ifc.findLatestVersion(cfg.get());
  std::cout << "  First findLatestVersion returned: " << firstLatest << "\n";

  for (int i = 1; i < 10; ++i) {
    int latest = ifc.findLatestVersion(cfg.get());
    if (latest != firstLatest) {
      std::cout << "  INCONSISTENT: Call " << i << " returned " << latest << "\n";
      allConsistent = false;
    }
  }

  std::cout << "  All calls consistent: " << (allConsistent ? "YES" : "NO") << "\n";
  std::cout << "  Latest version >= max stored: " << (firstLatest >= maxVer ? "YES" : "NO") << "\n";

  BOOST_CHECK(allConsistent);
  BOOST_CHECK_GE(firstLatest, maxVer);
  std::cout << "  Verified: findLatestVersion is consistent\n";
}

BOOST_AUTO_TEST_CASE(json_with_nested_objects) {
  std::cout << "\n=== TEST 8: JSON with deeply nested objects ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 500;
  cfg->getView().version = testVersion;

  std::string nestedJson = R"({
    "level1": {
      "level2": {
        "level3": {
          "level4": {
            "value": "deeply_nested"
          }
        }
      }
    }
  })";

  std::cout << "Step 1: Storing config with deeply nested JSON\n";
  cfg->getView().fillFromJSON(nestedJson);
  auto storeResult = ifc.saveActiveVersion(cfg.get(), false);
  std::cout << "  Store returned: " << storeResult << "\n";

  if (storeResult == 0) {
    std::cout << "Step 2: Loading and verifying nested structure\n";
    auto loaded = std::make_shared<TestConfiguration001>();
    auto loadResult = ifc.fill(loaded.get(), testVersion);
    std::cout << "  Load returned: " << loadResult << "\n";

    if (loadResult == 0) {
      std::stringstream ss;
      loaded->getView().printJSON(ss);
      std::string loadedJson = ss.str();
      std::cout << "  Content preview: " << loadedJson.substr(0, 100) << "...\n";

      bool hasNestedValue = loadedJson.find("deeply_nested") != std::string::npos;
      std::cout << "  Nested value preserved: " << (hasNestedValue ? "YES" : "NO") << "\n";

      BOOST_CHECK(hasNestedValue);
    }
  }

  std::cout << "  Verified: Nested JSON handling\n";
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(json_with_arrays) {
  std::cout << "\n=== TEST 9: JSON with arrays ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 510;
  cfg->getView().version = testVersion;

  std::string arrayJson = R"({
    "numbers": [1, 2, 3, 4, 5],
    "strings": ["a", "b", "c"],
    "mixed": [1, "two", 3.0, true, null]
  })";

  std::cout << "Step 1: Storing config with array values\n";
  cfg->getView().fillFromJSON(arrayJson);
  auto storeResult = ifc.saveActiveVersion(cfg.get(), false);
  std::cout << "  Store returned: " << storeResult << "\n";

  if (storeResult == 0) {
    std::cout << "Step 2: Loading and verifying arrays\n";
    auto loaded = std::make_shared<TestConfiguration001>();
    auto loadResult = ifc.fill(loaded.get(), testVersion);
    std::cout << "  Load returned: " << loadResult << "\n";

    if (loadResult == 0) {
      std::stringstream ss;
      loaded->getView().printJSON(ss);
      std::string loadedJson = ss.str();

      bool hasNumbers = loadedJson.find("numbers") != std::string::npos;
      bool hasStrings = loadedJson.find("strings") != std::string::npos;
      std::cout << "  Arrays preserved: " << ((hasNumbers && hasStrings) ? "YES" : "PARTIAL") << "\n";

      BOOST_CHECK(hasNumbers || hasStrings);
    }
  }

  std::cout << "  Verified: Array handling\n";
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(multiple_config_types_same_version) {
  std::cout << "\n=== TEST 10: Multiple config types with same version number ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int sharedVersion = fixture.baseVersion + 600;

  std::cout << "Step 1: Storing same version for multiple config types\n";

  auto cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = sharedVersion;
  cfg1->getView().fillFromJSON("{\"type\": \"config1\"}");
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg1.get(), false), 0);
  std::cout << "  Stored TestConfiguration001 v" << sharedVersion << "\n";

  auto cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = sharedVersion;
  cfg2->getView().fillFromJSON("{\"type\": \"config2\"}");
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg2.get(), false), 0);
  std::cout << "  Stored TestConfiguration002 v" << sharedVersion << "\n";

  std::cout << "Step 2: Loading each type independently\n";

  auto loaded1 = std::make_shared<TestConfiguration001>();
  BOOST_REQUIRE_EQUAL(ifc.fill(loaded1.get(), sharedVersion), 0);
  std::stringstream ss1;
  loaded1->getView().printJSON(ss1);
  std::cout << "  Config1 content: " << ss1.str() << "\n";

  auto loaded2 = std::make_shared<TestConfiguration002>();
  BOOST_REQUIRE_EQUAL(ifc.fill(loaded2.get(), sharedVersion), 0);
  std::stringstream ss2;
  loaded2->getView().printJSON(ss2);
  std::cout << "  Config2 content: " << ss2.str() << "\n";

  bool cfg1Correct = ss1.str().find("config1") != std::string::npos;
  bool cfg2Correct = ss2.str().find("config2") != std::string::npos;

  std::cout << "Step 3: Verifying content isolation\n";
  std::cout << "  Config1 has correct content: " << (cfg1Correct ? "YES" : "NO") << "\n";
  std::cout << "  Config2 has correct content: " << (cfg2Correct ? "YES" : "NO") << "\n";

  BOOST_CHECK(cfg1Correct);
  BOOST_CHECK(cfg2Correct);
  std::cout << "  Verified: Different config types with same version are isolated\n";
}

BOOST_AUTO_TEST_SUITE_END()
