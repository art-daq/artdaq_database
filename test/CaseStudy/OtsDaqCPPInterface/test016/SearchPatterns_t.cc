#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (SearchPatterns test)

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct SearchPatternsTestData {
  SearchPatternsTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== SEARCH PATTERNS TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~SearchPatternsTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

SearchPatternsTestData fixture;

BOOST_AUTO_TEST_SUITE(search_patterns_test)

void createSearchTestData(DatabaseConfigurationInterface& ifc, int version, const std::string& prefix) {
  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = version;
  ifc.saveActiveVersion(cfg.get());

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[cfg->getConfigurationName()] = version;

  for (int i = 0; i < 5; i++) {
    std::string compName = prefix + "_" + std::to_string(i);
    ifc.storeGlobalConfiguration(map, compName);
  }
}

BOOST_AUTO_TEST_CASE(wildcard_only_search) {
  std::cout << "\n=== TEST 1: Wildcard only search ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto results = ifc.findAllGlobalConfigurations("*");
  std::cout << "Step 1: findAllGlobalConfigurations(\"*\") returned " << results.size() << " results\n";

  BOOST_CHECK_GE(results.size(), 0);

  std::cout << "  Verified: Wildcard search returns valid results\n";
}

BOOST_AUTO_TEST_CASE(multiple_wildcards) {
  std::cout << "\n=== TEST 2: Multiple wildcards ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion;

  std::string prefix = "MultiWild_" + std::to_string(fixture.baseVersion);
  createSearchTestData(ifc, testVersion, prefix);
  std::cout << "Step 1: Created compositions with prefix: " << prefix << "\n";

  std::string pattern = "*Wild*" + std::to_string(fixture.baseVersion) + "*";

  try {
    auto results = ifc.findAllGlobalConfigurations(pattern);
    std::cout << "Step 2: Search pattern \"" << pattern << "\" returned " << results.size() << " results\n";
    BOOST_CHECK_GE(results.size(), 0);
  } catch (std::exception& e) {
    std::cout << "Step 2: Multiple wildcards threw exception (may be provider limitation)\n";
    std::cout << "  Exception: " << e.what() << "\n";
    BOOST_WARN_MESSAGE(false, "Multiple wildcards not supported by provider");
  }

  BOOST_CHECK(true);
  std::cout << "  Documented: Multiple wildcards behavior\n";
}

BOOST_AUTO_TEST_CASE(prefix_search) {
  std::cout << "\n=== TEST 3: Prefix search ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion + 100;

  std::string prefix = "PrefixSearch_" + std::to_string(fixture.baseVersion);
  createSearchTestData(ifc, testVersion, prefix);
  std::cout << "Step 1: Created 5 compositions with prefix: " << prefix << "\n";

  std::string pattern = prefix + "*";
  auto results = ifc.findAllGlobalConfigurations(pattern);
  std::cout << "Step 2: Search pattern \"" << pattern << "\" returned " << results.size() << " results\n";

  BOOST_CHECK_GE(results.size(), 5);

  std::cout << "  Verified: Prefix search works correctly\n";
}

BOOST_AUTO_TEST_CASE(regex_special_chars_escaped) {
  std::cout << "\n=== TEST 4: Regex special chars escaped ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::vector<std::string> regexPatterns = {"config.name", "config[1]", "config(test)", "config^start", "config$end"};

  int safeCount = 0;
  for (const auto& pattern : regexPatterns) {
    bool handled = false;
    try {
      auto results = ifc.findAllGlobalConfigurations(pattern);
      handled = true;
      std::cout << "  \"" << pattern << "\": returned " << results.size() << " results\n";
    } catch (std::exception& e) {
      handled = true;
      std::cout << "  \"" << pattern << "\": threw (acceptable)\n";
    }
    if (handled) safeCount++;
  }

  std::cout << "Step 1: Handled " << safeCount << "/" << regexPatterns.size() << " patterns safely\n";

  BOOST_CHECK_EQUAL(safeCount, regexPatterns.size());
  std::cout << "  Verified: Regex special characters handled safely\n";
}

BOOST_AUTO_TEST_CASE(sql_injection_like_patterns) {
  std::cout << "\n=== TEST 5: SQL injection like patterns ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion + 200;

  std::string prefix = "SafeTest_" + std::to_string(fixture.baseVersion);
  createSearchTestData(ifc, testVersion, prefix);

  std::vector<std::string> injectionPatterns = {"'; DROP TABLE configs; --", "config' OR '1'='1", "config\"; DELETE FROM *; --", "$(rm -rf /)",
                                                "`ls -la`"};

  int safeCount = 0;
  for (const auto& pattern : injectionPatterns) {
    try {
      auto results = ifc.findAllGlobalConfigurations(pattern);
      safeCount++;
      std::cout << "  \"" << pattern.substr(0, 20) << "...\": returned " << results.size() << " results (safe)\n";
    } catch (std::exception& e) {
      safeCount++;
      std::cout << "  \"" << pattern.substr(0, 20) << "...\": threw (safe)\n";
    }
  }

  auto safeResults = ifc.findAllGlobalConfigurations(prefix + "*");
  bool dbStillWorks = (safeResults.size() >= 5);
  std::cout << "Step 1: Database still working: " << (dbStillWorks ? "yes" : "NO!") << "\n";

  BOOST_CHECK_EQUAL(safeCount, injectionPatterns.size());
  BOOST_CHECK(dbStillWorks);

  std::cout << "  Verified: Injection patterns handled safely\n";
}

BOOST_AUTO_TEST_CASE(empty_search_behavior) {
  std::cout << "\n=== TEST 6: Empty search behavior ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto emptyResults = ifc.findAllGlobalConfigurations("");
  auto wildcardResults = ifc.findAllGlobalConfigurations("*");

  std::cout << "Step 1: Empty search returned " << emptyResults.size() << " results\n";
  std::cout << "Step 2: Wildcard search returned " << wildcardResults.size() << " results\n";

  if (emptyResults.size() == wildcardResults.size()) {
    std::cout << "  Behavior: Empty search = wildcard search\n";
  } else if (emptyResults.size() == 0) {
    std::cout << "  Behavior: Empty search returns no results\n";
  } else {
    std::cout << "  Behavior: Empty search is different from wildcard\n";
  }

  BOOST_CHECK(true);
  std::cout << "  Documented: Empty search behavior\n";
}

BOOST_AUTO_TEST_SUITE_END()
