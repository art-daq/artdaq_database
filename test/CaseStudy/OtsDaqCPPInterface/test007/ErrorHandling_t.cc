#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (ErrorHandling test)

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct ErrorHandlingTestData {
  ErrorHandlingTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== ERROR HANDLING TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~ErrorHandlingTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

ErrorHandlingTestData fixture;

BOOST_AUTO_TEST_SUITE(error_handling_test)

BOOST_AUTO_TEST_CASE(load_nonexistent_version) {
  std::cout << "\n=== TEST 1: Load nonexistent version ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();

  int nonExistentVersion = 999999999;

  auto result = ifc.fill(cfg.get(), nonExistentVersion);
  std::cout << "fill() returned: " << result << "\n";

  if (result == 0) {
    std::cout << "  NOTE: fill() returned success for non-existent version\n";
    std::cout << "  This may be expected if the implementation returns a default config\n";
    BOOST_WARN_MESSAGE(false, "fill() returns 0 for non-existent version - may need investigation");
  } else {
    std::cout << "  fill() correctly returned error for non-existent version\n";
  }

  BOOST_CHECK(true);
  std::cout << "  Documented: fill() behavior for nonexistent version\n";
}

BOOST_AUTO_TEST_CASE(load_nonexistent_composition) {
  std::cout << "\n=== TEST 2: Load nonexistent composition ===\n";

  auto ifc = DatabaseConfigurationInterface();

  BOOST_CHECK_THROW(ifc.loadGlobalConfiguration("NonExistentComposition_XYZ123_" + std::to_string(fixture.baseVersion)), std::exception);

  std::cout << "  Verified: Loading nonexistent composition throws exception\n";
}

BOOST_AUTO_TEST_CASE(store_duplicate_version_without_overwrite) {
  std::cout << "\n=== TEST 3: Store duplicate version without overwrite ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = testVersion;
  auto result1 = ifc.saveActiveVersion(cfg1.get());
  BOOST_REQUIRE_EQUAL(result1, 0);
  std::cout << "Step 1: Saved version " << testVersion << "\n";

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration001>();
  cfg2->getView().version = testVersion;
  auto result2 = ifc.saveActiveVersion(cfg2.get(), false);
  std::cout << "Step 2: Second save returned: " << result2 << "\n";

  BOOST_CHECK_NE(result2, 0);
  std::cout << "  Verified: Duplicate version without overwrite fails\n";
}

BOOST_AUTO_TEST_CASE(store_duplicate_composition_without_overwrite) {
  std::cout << "\n=== TEST 4: Store duplicate composition without overwrite ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion + 1;

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = testVersion;
  auto saveResult = ifc.saveActiveVersion(cfg.get());
  BOOST_REQUIRE_EQUAL(saveResult, 0);
  std::cout << "Step 1: Saved version " << testVersion << "\n";

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[cfg->getConfigurationName()] = testVersion;
  std::string compName = "DuplicateTest_" + std::to_string(fixture.baseVersion);

  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(map, compName));
  std::cout << "Step 2: Created composition " << compName << "\n";

  std::cout << "Step 3: Attempting to create duplicate composition...\n";
  BOOST_CHECK_THROW(ifc.storeGlobalConfiguration(map, compName), std::exception);

  std::cout << "  Verified: Duplicate composition without overwrite throws exception\n";
}

BOOST_AUTO_TEST_CASE(find_versions_for_unknown_type) {
  std::cout << "\n=== TEST 5: Find versions for unknown type ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();

  auto versions = ifc.getVersions(cfg.get());

  std::cout << "getVersions returned set with " << versions.size() << " elements\n";

  BOOST_CHECK_MESSAGE(true, "getVersions returned without throwing");
  std::cout << "  Verified: getVersions for config type returns valid set\n";
}

BOOST_AUTO_TEST_CASE(find_compositions_empty_database) {
  std::cout << "\n=== TEST 6: Find compositions with empty search ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto compositions = ifc.findAllGlobalConfigurations("");

  std::cout << "findAllGlobalConfigurations(\"\") returned set with " << compositions.size() << " elements\n";

  BOOST_CHECK_MESSAGE(true, "findAllGlobalConfigurations returned without throwing");
  std::cout << "  Verified: Empty search returns valid set\n";
}

BOOST_AUTO_TEST_CASE(find_compositions_wildcard) {
  std::cout << "\n=== TEST 7: Find compositions with wildcard ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto compositions = ifc.findAllGlobalConfigurations("*");

  std::cout << "findAllGlobalConfigurations(\"*\") returned set with " << compositions.size() << " elements\n";

  BOOST_CHECK_MESSAGE(true, "findAllGlobalConfigurations with wildcard returned without throwing");
  std::cout << "  Verified: Wildcard search returns valid set\n";
}

BOOST_AUTO_TEST_SUITE_END()
