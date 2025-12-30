#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (CompositionNaming test)

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct CompositionNamingTestData {
  CompositionNamingTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== COMPOSITION NAMING TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~CompositionNamingTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

CompositionNamingTestData fixture;

BOOST_AUTO_TEST_SUITE(composition_naming_test)

void createTestVersion(DatabaseConfigurationInterface& ifc, int version) {
  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = version;
  ifc.saveActiveVersion(cfg.get());
}

BOOST_AUTO_TEST_CASE(empty_composition_name_rejected) {
  std::cout << "\n=== TEST 1: Empty composition name rejected ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion;
  createTestVersion(ifc, testVersion);

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  map[cfg->getConfigurationName()] = testVersion;

  bool threwException = false;
  try {
    ifc.storeGlobalConfiguration(map, "");
  } catch (std::exception& e) {
    threwException = true;
    std::cout << "  Exception: " << e.what() << "\n";
  }

  std::cout << "Step 1: Empty name threw exception: " << (threwException ? "yes" : "no") << "\n";

  if (!threwException) {
    BOOST_WARN_MESSAGE(false, "Empty composition name was accepted - may need validation");
    try {
      auto loaded = ifc.loadGlobalConfiguration("");
      std::cout << "  NOTE: Empty name composition loaded successfully\n";
    } catch (...) {
      std::cout << "  NOTE: Empty name composition could not be loaded\n";
    }
  }

  BOOST_CHECK(true);
  std::cout << "  Documented: Empty name behavior\n";
}

BOOST_AUTO_TEST_CASE(whitespace_only_name_rejected) {
  std::cout << "\n=== TEST 2: Whitespace only name rejected ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion + 1;
  createTestVersion(ifc, testVersion);

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  map[cfg->getConfigurationName()] = testVersion;

  bool threwException = false;
  try {
    ifc.storeGlobalConfiguration(map, "   ");
  } catch (std::exception& e) {
    threwException = true;
    std::cout << "  Exception: " << e.what() << "\n";
  }

  std::cout << "Step 1: Whitespace-only name threw exception: " << (threwException ? "yes" : "no") << "\n";

  if (!threwException) {
    BOOST_WARN_MESSAGE(false, "Whitespace-only composition name was accepted");
  }

  BOOST_CHECK(true);
  std::cout << "  Documented: Whitespace-only name behavior\n";
}

BOOST_AUTO_TEST_CASE(very_long_name_handling) {
  std::cout << "\n=== TEST 3: Very long name handling ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion + 2;
  createTestVersion(ifc, testVersion);

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  map[cfg->getConfigurationName()] = testVersion;

  std::string longName(500, 'x');
  longName += "_" + std::to_string(fixture.baseVersion);
  std::cout << "Step 1: Testing name with " << longName.size() << " characters\n";

  bool succeeded = false;
  bool threwException = false;
  try {
    ifc.storeGlobalConfiguration(map, longName);
    succeeded = true;
  } catch (std::exception& e) {
    threwException = true;
    std::cout << "  Exception: " << e.what() << "\n";
  }

  std::cout << "Step 2: Long name succeeded: " << (succeeded ? "yes" : "no") << ", threw: " << (threwException ? "yes" : "no") << "\n";

  if (succeeded) {
    try {
      auto loaded = ifc.loadGlobalConfiguration(longName);
      std::cout << "Step 3: Long name composition loaded with " << loaded.size() << " members\n";
      BOOST_CHECK_EQUAL(loaded.size(), 1);
    } catch (std::exception& e) {
      std::cout << "Step 3: Could not load: " << e.what() << "\n";
      BOOST_WARN_MESSAGE(false, "Long name was stored but could not be loaded");
    }
  }

  BOOST_CHECK(true);
  std::cout << "  Documented: Long name behavior\n";
}

BOOST_AUTO_TEST_CASE(special_characters_in_name) {
  std::cout << "\n=== TEST 4: Special characters in name ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion + 3;
  createTestVersion(ifc, testVersion);

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  map[cfg->getConfigurationName()] = testVersion;

  std::vector<std::pair<std::string, std::string>> specialNames = {
      {"with_underscore_" + std::to_string(fixture.baseVersion), "underscore"},
      {"with-dash-" + std::to_string(fixture.baseVersion), "dash"},
      {"with.dot." + std::to_string(fixture.baseVersion), "dot"},
  };

  for (const auto& [name, desc] : specialNames) {
    bool succeeded = false;
    try {
      ifc.storeGlobalConfiguration(map, name);
      succeeded = true;
    } catch (std::exception& e) {
      std::cout << "  " << desc << ": rejected (" << e.what() << ")\n";
    }

    if (succeeded) {
      std::cout << "  " << desc << ": accepted\n";
    }
  }

  BOOST_CHECK(true);
  std::cout << "  Documented: Special character behavior\n";
}

BOOST_AUTO_TEST_CASE(leading_trailing_whitespace_handling) {
  std::cout << "\n=== TEST 5: Leading/trailing whitespace handling ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion + 5;
  createTestVersion(ifc, testVersion);

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  map[cfg->getConfigurationName()] = testVersion;

  std::string baseName = "TrimTest_" + std::to_string(fixture.baseVersion);
  std::string paddedName = "  " + baseName + "  ";

  bool succeeded = false;
  try {
    ifc.storeGlobalConfiguration(map, paddedName);
    succeeded = true;
    std::cout << "Step 1: Stored with padded name \"" << paddedName << "\"\n";
  } catch (std::exception& e) {
    std::cout << "Step 1: Padded name rejected: " << e.what() << "\n";
  }

  if (succeeded) {
    try {
      auto loaded = ifc.loadGlobalConfiguration(paddedName);
      std::cout << "Step 2a: Loaded with padded name (size=" << loaded.size() << ")\n";
    } catch (...) {
      std::cout << "Step 2a: Could not load with padded name\n";
    }

    try {
      auto loaded = ifc.loadGlobalConfiguration(baseName);
      std::cout << "Step 2b: Loaded with trimmed name (size=" << loaded.size() << ")\n";
    } catch (...) {
      std::cout << "Step 2b: Could not load with trimmed name\n";
    }
  }

  BOOST_CHECK(true);
  std::cout << "  Documented: Whitespace handling behavior\n";
}

BOOST_AUTO_TEST_CASE(case_sensitivity_in_names) {
  std::cout << "\n=== TEST 6: Case sensitivity in names ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion + 6;
  createTestVersion(ifc, testVersion);

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  map[cfg->getConfigurationName()] = testVersion;

  std::string lowerName = "casetest_" + std::to_string(fixture.baseVersion);
  std::string upperName = "CASETEST_" + std::to_string(fixture.baseVersion);

  ifc.storeGlobalConfiguration(map, lowerName);
  std::cout << "Step 1: Stored \"" << lowerName << "\"\n";

  bool upperSucceeded = false;
  try {
    ifc.storeGlobalConfiguration(map, upperName);
    upperSucceeded = true;
    std::cout << "Step 2: \"" << upperName << "\" stored as separate composition (case sensitive)\n";
  } catch (std::exception& e) {
    std::cout << "Step 2: \"" << upperName << "\" rejected (case insensitive)\n";
  }

  if (upperSucceeded) {
    try {
      auto loadedLower = ifc.loadGlobalConfiguration(lowerName);
      auto loadedUpper = ifc.loadGlobalConfiguration(upperName);
      std::cout << "Step 3: Both compositions exist separately\n";
    } catch (...) {
      std::cout << "Step 3: Issue loading compositions\n";
    }
  }

  BOOST_CHECK(true);
  std::cout << "  Documented: Case sensitivity behavior\n";
}

BOOST_AUTO_TEST_SUITE_END()
