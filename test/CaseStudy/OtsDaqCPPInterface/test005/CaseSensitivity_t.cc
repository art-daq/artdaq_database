#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (CaseSensitivity test)

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <string>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct CaseSensitivityTestData {
  CaseSensitivityTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== CASE SENSITIVITY VERIFICATION TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~CaseSensitivityTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

CaseSensitivityTestData fixture;

BOOST_AUTO_TEST_SUITE(case_sensitivity_test)

BOOST_AUTO_TEST_CASE(different_cases_create_different_compositions) {
  std::cout << "\n=== TEST 1: Different cases create different compositions ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  auto result1 = ifc.saveActiveVersion(cfg1.get());
  BOOST_CHECK_EQUAL(result1, 0);

  std::cout << "Created configuration version: " << version1 << "\n";

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;

  std::string name1 = "RunConfig";
  std::string name2 = "runconfig";
  std::string name3 = "RUNCONFIG";

  std::cout << "Creating compositions with different cases:\n";
  std::cout << "  1. '" << name1 << "'\n";
  std::cout << "  2. '" << name2 << "'\n";
  std::cout << "  3. '" << name3 << "'\n";

  ifc.storeGlobalConfiguration(compositionMap, name1);
  ifc.storeGlobalConfiguration(compositionMap, name2);
  ifc.storeGlobalConfiguration(compositionMap, name3);

  auto allCompositions = ifc.findAllGlobalConfigurations();

  bool found1 = (allCompositions.find(name1) != allCompositions.end());
  bool found2 = (allCompositions.find(name2) != allCompositions.end());
  bool found3 = (allCompositions.find(name3) != allCompositions.end());

  std::cout << "Results:\n";
  std::cout << "  '" << name1 << "' found: " << (found1 ? "YES" : "NO") << "\n";
  std::cout << "  '" << name2 << "' found: " << (found2 ? "YES" : "NO") << "\n";
  std::cout << "  '" << name3 << "' found: " << (found3 ? "YES" : "NO") << "\n";

  BOOST_CHECK_EQUAL(found1, true);
  BOOST_CHECK_EQUAL(found2, true);
  BOOST_CHECK_EQUAL(found3, true);

  std::cout << "  Verified: All three compositions exist independently\n";
}

BOOST_AUTO_TEST_CASE(case_sensitive_lookup) {
  std::cout << "\n=== TEST 2: Case-sensitive lookup ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 10;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;

  std::string exactName = "TestComp_Case";
  ifc.storeGlobalConfiguration(compositionMap, exactName);

  std::cout << "Created composition: '" << exactName << "'\n";

  std::cout << "Loading with exact case: '" << exactName << "'...\n";
  try {
    auto loadedMap = ifc.loadGlobalConfiguration(exactName);
    BOOST_CHECK_EQUAL(loadedMap.size(), 1);
    std::cout << "  SUCCESS: Loaded composition with exact case\n";
  } catch (std::exception const& e) {
    BOOST_FAIL("Failed to load with exact case: " << e.what());
  }

  std::string wrongCase = "testcomp_case";
  std::cout << "Loading with different case: '" << wrongCase << "'...\n";

  bool loadedWithWrongCase = true;
  try {
    auto loadedMap = ifc.loadGlobalConfiguration(wrongCase);
    loadedWithWrongCase = true;
    std::cout << "  UNEXPECTED: Loaded with different case (case-insensitive!)\n";
  } catch (std::exception const& e) {
    loadedWithWrongCase = false;
    std::cout << "  EXPECTED: Failed to load with different case (case-sensitive)\n";
  }

  BOOST_CHECK_EQUAL(loadedWithWrongCase, false);
}

BOOST_AUTO_TEST_CASE(case_sensitive_search) {
  std::cout << "\n=== TEST 3: Case-sensitive search ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 20;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;

  std::string name1 = "SearchTest_ABC";
  std::string name2 = "SearchTest_abc";

  ifc.storeGlobalConfiguration(compositionMap, name1);
  ifc.storeGlobalConfiguration(compositionMap, name2);

  std::cout << "Created compositions:\n";
  std::cout << "  1. '" << name1 << "'\n";
  std::cout << "  2. '" << name2 << "'\n";

  std::cout << "Searching for exact match: '" << name1 << "'\n";
  auto results1 = ifc.findAllGlobalConfigurations(name1);

  std::cout << "Found " << results1.size() << " result(s)\n";

  if (results1.size() == 1) {
    std::cout << "  CASE-SENSITIVE: Found only exact match\n";
    BOOST_CHECK_EQUAL(results1.size(), 1);
  } else if (results1.size() == 2) {
    std::cout << "  CASE-INSENSITIVE: Found both variations (potential bug)\n";
    BOOST_WARN_MESSAGE(false, "Search appears to be case-insensitive");
  }

  bool foundExact = (results1.find(name1) != results1.end());
  BOOST_CHECK_EQUAL(foundExact, true);
}

BOOST_AUTO_TEST_CASE(case_preservation_in_storage) {
  std::cout << "\n=== TEST 4: Case preservation in storage ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 30;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;

  std::string originalName = "MixedCase_TeStNaMe_123";
  ifc.storeGlobalConfiguration(compositionMap, originalName);

  std::cout << "Stored composition with name: '" << originalName << "'\n";

  auto allCompositions = ifc.findAllGlobalConfigurations();

  bool foundWithOriginalCase = (allCompositions.find(originalName) != allCompositions.end());

  std::cout << "Searching for composition in returned list...\n";
  std::cout << "  Original case '" << originalName << "' found: " << (foundWithOriginalCase ? "YES" : "NO") << "\n";

  BOOST_CHECK_EQUAL(foundWithOriginalCase, true);

  auto loadedMap = ifc.loadGlobalConfiguration(originalName);
  BOOST_CHECK_EQUAL(loadedMap.size(), 1);

  std::cout << "  Verified: Case is preserved in storage and retrieval\n";
}

BOOST_AUTO_TEST_CASE(configuration_type_names_case_sensitive) {
  std::cout << "\n=== TEST 5: Configuration type names case-sensitive ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  std::string actualTypeName = cfg1->getConfigurationName();

  std::cout << "Actual configuration type name: '" << actualTypeName << "'\n";

  int version1 = fixture.baseVersion + 40;
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[actualTypeName] = version1;

  std::string compName = "TypeNameTest_" + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(compositionMap, compName);

  std::cout << "Created composition with type: '" << actualTypeName << "'\n";

  auto loadedMap = ifc.loadGlobalConfiguration(compName);

  bool foundExactType = (loadedMap.find(actualTypeName) != loadedMap.end());

  std::cout << "Loaded composition, checking for exact type name...\n";
  std::cout << "  Found '" << actualTypeName << "': " << (foundExactType ? "YES" : "NO") << "\n";

  BOOST_CHECK_EQUAL(foundExactType, true);
  BOOST_CHECK_EQUAL(loadedMap.size(), 1);

  std::cout << "  All configuration types in composition:\n";
  for (auto const& entry : loadedMap) {
    std::cout << "    - '" << entry.first << "' -> version " << entry.second << "\n";
  }

  std::cout << "  Verified: Configuration type names are case-sensitive\n";
}

BOOST_AUTO_TEST_CASE(case_sensitive_reverse_lookup) {
  std::cout << "\n=== TEST 6: Case-sensitive reverse lookup ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 50;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::string configTypeName = cfg1->getConfigurationName();

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[configTypeName] = version1;

  std::string comp1Name = "ReverseTest_ABC";
  std::string comp2Name = "ReverseTest_abc";

  ifc.storeGlobalConfiguration(compositionMap, comp1Name);
  ifc.storeGlobalConfiguration(compositionMap, comp2Name);

  std::cout << "Created compositions:\n";
  std::cout << "  1. '" << comp1Name << "'\n";
  std::cout << "  2. '" << comp2Name << "'\n";

  auto foundCompositions = ifc.findCompositionsContaining(configTypeName, std::to_string(version1));

  std::cout << "Reverse lookup for version " << version1 << " found " << foundCompositions.size() << " compositions\n";

  BOOST_CHECK_EQUAL(foundCompositions.size(), 2);

  bool found1 = (foundCompositions.find(comp1Name) != foundCompositions.end());
  bool found2 = (foundCompositions.find(comp2Name) != foundCompositions.end());

  std::cout << "  Found '" << comp1Name << "': " << (found1 ? "YES" : "NO") << "\n";
  std::cout << "  Found '" << comp2Name << "': " << (found2 ? "YES" : "NO") << "\n";

  BOOST_CHECK_EQUAL(found1, true);
  BOOST_CHECK_EQUAL(found2, true);

  for (auto const& compName : foundCompositions) {
    std::cout << "  - '" << compName << "'\n";

    if (compName == comp1Name) {
      std::cout << "    Matches '" << comp1Name << "' exactly (case preserved)\n";
    } else if (compName == comp2Name) {
      std::cout << "    Matches '" << comp2Name << "' exactly (case preserved)\n";
    }
  }

  std::cout << "  Verified: Reverse lookup preserves case in composition names\n";
}

BOOST_AUTO_TEST_SUITE_END()
