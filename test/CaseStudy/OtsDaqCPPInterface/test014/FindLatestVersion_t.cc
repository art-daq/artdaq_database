#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (FindLatestVersion test)

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct FindLatestVersionTestData {
  FindLatestVersionTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== FIND LATEST VERSION TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~FindLatestVersionTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

FindLatestVersionTestData fixture;

BOOST_AUTO_TEST_SUITE(find_latest_version_test)

BOOST_AUTO_TEST_CASE(findlatest_single_version) {
  std::cout << "\n=== TEST 1: findLatest single version ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion;

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = testVersion;
  auto saveResult = ifc.saveActiveVersion(cfg.get());
  BOOST_REQUIRE_EQUAL(saveResult, 0);
  std::cout << "Step 1: Saved version " << testVersion << "\n";

  auto latest = ifc.findLatestVersion(cfg.get());
  std::cout << "Step 2: findLatestVersion returned: " << latest << "\n";

  BOOST_CHECK_GE(latest, testVersion);
  std::cout << "  Verified: findLatestVersion works with single version\n";
}

BOOST_AUTO_TEST_CASE(findlatest_ascending_versions) {
  std::cout << "\n=== TEST 2: findLatest ascending versions ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int baseV = fixture.baseVersion + 100;

  for (int i = 1; i <= 10; i++) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = baseV + i;
    auto result = ifc.saveActiveVersion(cfg.get());
    BOOST_REQUIRE_EQUAL(result, 0);
  }
  std::cout << "Step 1: Created versions " << baseV + 1 << " to " << baseV + 10 << "\n";

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  auto latest = ifc.findLatestVersion(cfg.get());
  std::cout << "Step 2: findLatestVersion returned: " << latest << "\n";

  BOOST_CHECK_GE(latest, baseV + 10);
  std::cout << "  Verified: findLatestVersion returns highest ascending version\n";
}

BOOST_AUTO_TEST_CASE(findlatest_descending_versions) {
  std::cout << "\n=== TEST 3: findLatest descending versions ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int baseV = fixture.baseVersion + 200;

  for (int i = 10; i >= 1; i--) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = baseV + i;
    auto result = ifc.saveActiveVersion(cfg.get());
    BOOST_REQUIRE_EQUAL(result, 0);
  }
  std::cout << "Step 1: Created versions " << baseV + 10 << " down to " << baseV + 1 << " (descending insert order)\n";

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  auto latest = ifc.findLatestVersion(cfg.get());
  std::cout << "Step 2: findLatestVersion returned: " << latest << "\n";

  BOOST_CHECK_GE(latest, baseV + 10);
  std::cout << "  Verified: findLatestVersion returns highest regardless of insertion order\n";
}

BOOST_AUTO_TEST_CASE(findlatest_random_order_versions) {
  std::cout << "\n=== TEST 4: findLatest random order versions ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int baseV = fixture.baseVersion + 300;

  std::vector<int> versions = {50, 10, 90, 30, 70, 20, 80, 40, 60, 100};

  for (int v : versions) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = baseV + v;
    auto result = ifc.saveActiveVersion(cfg.get());
    BOOST_REQUIRE_EQUAL(result, 0);
  }
  std::cout << "Step 1: Created 10 versions in random order, highest is " << baseV + 100 << "\n";

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  auto latest = ifc.findLatestVersion(cfg.get());
  std::cout << "Step 2: findLatestVersion returned: " << latest << "\n";

  BOOST_CHECK_GE(latest, baseV + 100);
  std::cout << "  Verified: findLatestVersion returns highest with random insertion\n";
}

BOOST_AUTO_TEST_CASE(findlatest_with_large_gaps) {
  std::cout << "\n=== TEST 5: findLatest with large gaps ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int baseV = fixture.baseVersion + 500;

  std::vector<int> versions = {1, 100, 10000, 100000};

  for (int v : versions) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = baseV + v;
    auto result = ifc.saveActiveVersion(cfg.get());
    BOOST_REQUIRE_EQUAL(result, 0);
  }
  std::cout << "Step 1: Created versions with large gaps: " << baseV + 1 << ", " << baseV + 100 << ", " << baseV + 10000 << ", " << baseV + 100000
            << "\n";

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  auto latest = ifc.findLatestVersion(cfg.get());
  std::cout << "Step 2: findLatestVersion returned: " << latest << "\n";

  BOOST_CHECK_GE(latest, baseV + 100000);
  std::cout << "  Verified: findLatestVersion handles large gaps correctly\n";
}

BOOST_AUTO_TEST_CASE(findlatest_different_config_types_independent) {
  std::cout << "\n=== TEST 6: findLatest different config types independent ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int baseV = fixture.baseVersion + 600;

  for (int v = 1; v <= 3; v++) {
    std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
    cfg1->getView().version = baseV + v;
    auto result = ifc.saveActiveVersion(cfg1.get());
    BOOST_REQUIRE_EQUAL(result, 0);
  }
  std::cout << "Step 1: TestConfiguration001 versions: " << baseV + 1 << "-" << baseV + 3 << "\n";

  for (int v = 100; v <= 300; v += 100) {
    std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
    cfg2->getView().version = baseV + v;
    auto result = ifc.saveActiveVersion(cfg2.get());
    BOOST_REQUIRE_EQUAL(result, 0);
  }
  std::cout << "Step 2: TestConfiguration002 versions: " << baseV + 100 << ", " << baseV + 200 << ", " << baseV + 300 << "\n";

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();

  auto latest1 = ifc.findLatestVersion(cfg1.get());
  auto latest2 = ifc.findLatestVersion(cfg2.get());

  std::cout << "Step 3: Type1 latest: " << latest1 << ", Type2 latest: " << latest2 << "\n";

  BOOST_CHECK_GE(latest1, baseV + 3);
  BOOST_CHECK_GE(latest2, baseV + 300);
  BOOST_CHECK_NE(latest1, latest2);

  std::cout << "  Verified: findLatestVersion is independent per config type\n";
}

BOOST_AUTO_TEST_CASE(findlatest_consecutive_calls_consistent) {
  std::cout << "\n=== TEST 7: findLatest consecutive calls consistent ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int baseV = fixture.baseVersion + 700;

  for (int v = 1; v <= 5; v++) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = baseV + v;
    auto result = ifc.saveActiveVersion(cfg.get());
    BOOST_REQUIRE_EQUAL(result, 0);
  }
  std::cout << "Step 1: Created versions " << baseV + 1 << " to " << baseV + 5 << "\n";

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  int firstLatest = ifc.findLatestVersion(cfg.get());

  int consistentCount = 0;
  for (int i = 0; i < 10; i++) {
    int latest = ifc.findLatestVersion(cfg.get());
    if (latest == firstLatest) {
      consistentCount++;
    }
  }

  std::cout << "Step 2: First call returned: " << firstLatest << ", consistent: " << consistentCount << "/10\n";

  BOOST_CHECK_EQUAL(consistentCount, 10);
  std::cout << "  Verified: findLatestVersion returns consistent results\n";
}

BOOST_AUTO_TEST_SUITE_END()
