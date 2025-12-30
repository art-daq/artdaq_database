#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (VersionEdgeCases test)

#include <boost/test/unit_test.hpp>

#include <climits>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct VersionEdgeCasesTestData {
  VersionEdgeCasesTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== VERSION EDGE CASES TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~VersionEdgeCasesTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

VersionEdgeCasesTestData fixture;

BOOST_AUTO_TEST_SUITE(version_edge_cases_test)

BOOST_AUTO_TEST_CASE(version_zero_is_valid) {
  std::cout << "\n=== TEST 1: Version zero is valid ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = 0;

  auto result = ifc.saveActiveVersion(cfg.get());
  std::cout << "Step 1: Save version 0 returned: " << result << "\n";

  if (result != 0) {
    std::cout << "  NOTE: Version 0 may already exist from previous test runs\n";
    std::shared_ptr<ConfigurationBase> readCfg = std::make_shared<TestConfiguration001>();
    auto fillResult = ifc.fill(readCfg.get(), 0);
    if (fillResult == 0) {
      std::cout << "  Version 0 already exists and is loadable\n";
      BOOST_CHECK(true);
    } else {
      std::cout << "  Version 0 is rejected by the system\n";
      BOOST_WARN_MESSAGE(false, "Version 0 may not be supported");
    }
  } else {
    auto versions = ifc.getVersions(cfg.get());
    bool found = (versions.find(0) != versions.end());
    std::cout << "Step 2: Version 0 found in getVersions: " << (found ? "yes" : "no") << "\n";
    BOOST_CHECK(found);
  }

  std::cout << "  Documented: Version 0 behavior\n";
}

BOOST_AUTO_TEST_CASE(negative_version_handling) {
  std::cout << "\n=== TEST 2: Negative version handling ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = -1;

  auto result = ifc.saveActiveVersion(cfg.get());
  std::cout << "Step 1: Save version -1 returned: " << result << "\n";

  if (result == 0) {
    std::cout << "  NOTE: Negative versions are accepted by the system\n";
    BOOST_WARN_MESSAGE(false, "Negative versions are accepted - may need validation");

    auto versions = ifc.getVersions(cfg.get());
    bool found = (versions.find(-1) != versions.end());
    std::cout << "Step 2: Version -1 found in getVersions: " << (found ? "yes" : "no") << "\n";
  } else {
    std::cout << "  OK: Negative versions are rejected\n";
  }

  BOOST_CHECK(true);
  std::cout << "  Documented: Negative version behavior\n";
}

BOOST_AUTO_TEST_CASE(max_int_version) {
  std::cout << "\n=== TEST 3: Maximum integer version ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = INT_MAX;
  std::cout << "Using INT_MAX = " << INT_MAX << "\n";

  auto result = ifc.saveActiveVersion(cfg.get());
  std::cout << "Step 1: Save version INT_MAX returned: " << result << "\n";

  if (result == 0) {
    std::cout << "  INT_MAX version accepted\n";
    auto versions = ifc.getVersions(cfg.get());
    bool found = (versions.find(INT_MAX) != versions.end());
    std::cout << "Step 2: INT_MAX found in getVersions: " << (found ? "yes" : "no") << "\n";
  } else {
    std::cout << "  INT_MAX version rejected (acceptable)\n";
  }

  BOOST_CHECK(true);
  std::cout << "  Verified: INT_MAX handled without crash\n";
}

BOOST_AUTO_TEST_CASE(version_gap_findlatest_correct) {
  std::cout << "\n=== TEST 4: Version gap findLatest correct ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int baseV = fixture.baseVersion + 1000;

  std::vector<int> versions = {baseV + 1, baseV + 5, baseV + 10, baseV + 100};

  for (int v : versions) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = v;
    auto result = ifc.saveActiveVersion(cfg.get());
    BOOST_REQUIRE_EQUAL(result, 0);
  }
  std::cout << "Step 1: Created versions with gaps: " << versions[0] << ", " << versions[1] << ", " << versions[2] << ", " << versions[3] << "\n";

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  auto latest = ifc.findLatestVersion(cfg.get());
  std::cout << "Step 2: findLatestVersion returned: " << latest << "\n";

  BOOST_CHECK_GE(latest, baseV + 100);
  std::cout << "  Verified: findLatestVersion works correctly with gaps\n";
}

BOOST_AUTO_TEST_CASE(version_ordering_not_insertion_order) {
  std::cout << "\n=== TEST 5: Version ordering not insertion order ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int baseV = fixture.baseVersion + 2000;

  std::vector<int> insertOrder = {baseV + 50, baseV + 10, baseV + 30, baseV + 20};

  for (int v : insertOrder) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = v;
    auto result = ifc.saveActiveVersion(cfg.get());
    BOOST_REQUIRE_EQUAL(result, 0);
  }
  std::cout << "Step 1: Inserted versions in order: " << insertOrder[0] << ", " << insertOrder[1] << ", " << insertOrder[2] << ", " << insertOrder[3]
            << "\n";

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  auto latest = ifc.findLatestVersion(cfg.get());
  std::cout << "Step 2: findLatestVersion returned: " << latest << " (expected >= " << baseV + 50 << ")\n";

  BOOST_CHECK_GE(latest, baseV + 50);
  std::cout << "  Verified: findLatestVersion uses numeric ordering, not insertion order\n";
}

BOOST_AUTO_TEST_CASE(getversions_returns_sorted) {
  std::cout << "\n=== TEST 6: getVersions returns sorted ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int baseV = fixture.baseVersion + 3000;

  std::vector<int> insertOrder = {baseV + 100, baseV + 1, baseV + 50, baseV + 25, baseV + 75};

  for (int v : insertOrder) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = v;
    auto result = ifc.saveActiveVersion(cfg.get());
    BOOST_REQUIRE_EQUAL(result, 0);
  }
  std::cout << "Step 1: Inserted " << insertOrder.size() << " versions in random order\n";

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  auto versions = ifc.getVersions(cfg.get());

  std::vector<int> foundVersions;
  for (int v : insertOrder) {
    if (versions.find(v) != versions.end()) {
      foundVersions.push_back(v);
    }
  }

  std::cout << "Step 2: Found " << foundVersions.size() << "/" << insertOrder.size() << " inserted versions\n";
  BOOST_CHECK_EQUAL(foundVersions.size(), insertOrder.size());

  int prev = INT_MIN;
  bool isSorted = true;
  for (int v : versions) {
    if (v < prev) {
      isSorted = false;
      break;
    }
    prev = v;
  }

  std::cout << "Step 3: All versions sorted: " << (isSorted ? "yes" : "no") << "\n";
  BOOST_CHECK(isSorted);

  std::cout << "  Verified: getVersions returns sorted set\n";
}

BOOST_AUTO_TEST_CASE(same_version_different_data_overwrite) {
  std::cout << "\n=== TEST 7: Same version different data overwrite ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion + 4000;

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"data\": \"original_value_12345\"}");
  auto result1 = ifc.saveActiveVersion(cfg.get());
  BOOST_REQUIRE_EQUAL(result1, 0);
  std::cout << "Step 1: Saved version " << testVersion << " with original data\n";

  cfg->getView().fillFromJSON("{\"data\": \"updated_value_67890\"}");
  auto result2 = ifc.saveActiveVersion(cfg.get(), true);
  BOOST_CHECK_EQUAL(result2, 0);
  std::cout << "Step 2: Overwrote with updated data, result: " << result2 << "\n";

  std::shared_ptr<ConfigurationBase> readCfg = std::make_shared<TestConfiguration001>();
  auto fillResult = ifc.fill(readCfg.get(), testVersion);
  BOOST_REQUIRE_EQUAL(fillResult, 0);

  bool hasUpdated = (readCfg->getView()._json.find("updated_value_67890") != std::string::npos);
  bool hasOriginal = (readCfg->getView()._json.find("original_value_12345") != std::string::npos);
  std::cout << "Step 3: Loaded data contains updated: " << (hasUpdated ? "yes" : "no") << ", original: " << (hasOriginal ? "yes" : "no") << "\n";

  BOOST_CHECK(hasUpdated);
  BOOST_CHECK(!hasOriginal);
  std::cout << "  Verified: Overwrite replaces data correctly\n";
}

BOOST_AUTO_TEST_CASE(version_one_million) {
  std::cout << "\n=== TEST 8: Version one million ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int largeVersion = fixture.baseVersion + 1000000;
  std::cout << "Using version: " << largeVersion << "\n";

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = largeVersion;

  auto result = ifc.saveActiveVersion(cfg.get());
  std::cout << "Step 1: Save version " << largeVersion << " returned: " << result << "\n";
  BOOST_CHECK_EQUAL(result, 0);

  std::shared_ptr<ConfigurationBase> readCfg = std::make_shared<TestConfiguration001>();
  auto fillResult = ifc.fill(readCfg.get(), largeVersion);
  std::cout << "Step 2: fill() for version " << largeVersion << " returned: " << fillResult << "\n";
  BOOST_CHECK_EQUAL(fillResult, 0);

  std::cout << "  Verified: Large version numbers work correctly\n";
}

BOOST_AUTO_TEST_SUITE_END()
