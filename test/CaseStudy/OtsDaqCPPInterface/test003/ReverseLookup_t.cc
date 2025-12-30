#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (ReverseLookup test)

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <string>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct ReverseLookupTestData {
  ReverseLookupTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== REVERSE LOOKUP TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~ReverseLookupTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

ReverseLookupTestData fixture;

BOOST_AUTO_TEST_SUITE(reverse_lookup_test)

BOOST_AUTO_TEST_CASE(basic_reverse_lookup) {
  std::cout << "\n=== TEST 1: Basic reverse lookup ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion;
  int version2 = fixture.baseVersion + 1;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  auto result1 = ifc.saveActiveVersion(cfg1.get());
  BOOST_CHECK_EQUAL(result1, 0);

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = version2;
  auto result2 = ifc.saveActiveVersion(cfg2.get());
  BOOST_CHECK_EQUAL(result2, 0);

  std::cout << "Created versions: " << version1 << ", " << version2 << "\n";

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;
  compositionMap[cfg2->getConfigurationName()] = version2;

  std::string compositionName = std::string{"TestComp_"} + std::to_string(fixture.baseVersion);

  ifc.storeGlobalConfiguration(compositionMap, compositionName);
  std::cout << "Created composition: " << compositionName << "\n";

  std::cout << "Performing reverse lookup for " << cfg1->getConfigurationName() << " v" << version1 << "\n";

  auto foundCompositions = ifc.findCompositionsContaining(cfg1->getConfigurationName(), std::to_string(version1));

  std::cout << "Found " << foundCompositions.size() << " composition(s)\n";
  for (auto const& comp : foundCompositions) {
    std::cout << "  - " << comp << "\n";
  }

  bool found = (foundCompositions.find(compositionName) != foundCompositions.end());

  BOOST_CHECK_EQUAL(found, true);
  std::cout << "  Verified: Composition found via reverse lookup\n";
}

BOOST_AUTO_TEST_CASE(multiple_compositions_same_version) {
  std::cout << "\n=== TEST 2: Multiple compositions same version ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int sharedVersion = fixture.baseVersion + 10;

  std::shared_ptr<ConfigurationBase> cfgShared = std::make_shared<TestConfiguration001>();
  cfgShared->getView().version = sharedVersion;
  ifc.saveActiveVersion(cfgShared.get());

  std::cout << "Created shared version: " << sharedVersion << "\n";

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[cfgShared->getConfigurationName()] = sharedVersion;

  std::string comp1Name = std::string{"Comp1_"} + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(map1, comp1Name);
  std::cout << "Created composition 1: " << comp1Name << "\n";

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[cfgShared->getConfigurationName()] = sharedVersion;

  std::string comp2Name = std::string{"Comp2_"} + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(map2, comp2Name);
  std::cout << "Created composition 2: " << comp2Name << "\n";

  auto foundCompositions = ifc.findCompositionsContaining(cfgShared->getConfigurationName(), std::to_string(sharedVersion));

  std::cout << "Reverse lookup found " << foundCompositions.size() << " compositions\n";

  BOOST_CHECK_EQUAL(foundCompositions.size(), 2);

  bool found1 = (foundCompositions.find(comp1Name) != foundCompositions.end());
  bool found2 = (foundCompositions.find(comp2Name) != foundCompositions.end());

  BOOST_CHECK_EQUAL(found1, true);
  BOOST_CHECK_EQUAL(found2, true);

  std::cout << "  Verified: Both compositions found\n";
}

BOOST_AUTO_TEST_CASE(reverse_lookup_no_matches) {
  std::cout << "\n=== TEST 3: Reverse lookup no matches ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int orphanVersion = fixture.baseVersion + 100;

  std::shared_ptr<ConfigurationBase> cfgOrphan = std::make_shared<TestConfiguration001>();
  cfgOrphan->getView().version = orphanVersion;
  ifc.saveActiveVersion(cfgOrphan.get());

  std::cout << "Created orphan version (not in any composition): " << orphanVersion << "\n";

  auto foundCompositions = ifc.findCompositionsContaining(cfgOrphan->getConfigurationName(), std::to_string(orphanVersion));

  std::cout << "Reverse lookup found " << foundCompositions.size() << " compositions\n";

  BOOST_CHECK_EQUAL(foundCompositions.size(), 0);
  std::cout << "  Verified: No compositions found (as expected)\n";
}

BOOST_AUTO_TEST_CASE(version_multiple_positions_same_composition) {
  std::cout << "\n=== TEST 4: Version multiple positions same composition ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 200;
  int version2 = fixture.baseVersion + 201;

  std::shared_ptr<ConfigurationBase> cfg1a = std::make_shared<TestConfiguration001>();
  cfg1a->getView().version = version1;
  ifc.saveActiveVersion(cfg1a.get());

  std::shared_ptr<ConfigurationBase> cfg1b = std::make_shared<TestConfiguration001>();
  cfg1b->getView().version = version2;
  ifc.saveActiveVersion(cfg1b.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1a->getConfigurationName()] = version1;

  std::string compName = std::string{"SingleTypeComp_"} + std::to_string(fixture.baseVersion);

  ifc.storeGlobalConfiguration(compositionMap, compName);

  auto found1 = ifc.findCompositionsContaining(cfg1a->getConfigurationName(), std::to_string(version1));

  BOOST_CHECK_EQUAL(found1.size(), 1);

  auto found2 = ifc.findCompositionsContaining(cfg1b->getConfigurationName(), std::to_string(version2));

  BOOST_CHECK_EQUAL(found2.size(), 0);

  std::cout << "  Verified: Only the stored version is found\n";
}

BOOST_AUTO_TEST_CASE(performance_many_compositions) {
  std::cout << "\n=== TEST 5: Performance many compositions ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int targetVersion = fixture.baseVersion + 500;

  std::shared_ptr<ConfigurationBase> cfgTarget = std::make_shared<TestConfiguration001>();
  cfgTarget->getView().version = targetVersion;
  ifc.saveActiveVersion(cfgTarget.get());

  std::string targetCompName;

  for (int i = 0; i < 10; ++i) {
    int otherVersion = fixture.baseVersion + 600 + i;

    std::shared_ptr<ConfigurationBase> cfgOther = std::make_shared<TestConfiguration002>();
    cfgOther->getView().version = otherVersion;
    ifc.saveActiveVersion(cfgOther.get());

    auto map = DatabaseConfigurationInterface::config_version_map_t{};

    if (i == 5) {
      map[cfgTarget->getConfigurationName()] = targetVersion;
      map[cfgOther->getConfigurationName()] = otherVersion;
      targetCompName = std::string{"PerfComp_"} + std::to_string(i) + "_" + std::to_string(fixture.baseVersion);
    } else {
      map[cfgOther->getConfigurationName()] = otherVersion;
    }

    std::string compName = std::string{"PerfComp_"} + std::to_string(i) + "_" + std::to_string(fixture.baseVersion);
    ifc.storeGlobalConfiguration(map, compName);
  }

  std::cout << "Created 10 compositions, target is in: " << targetCompName << "\n";

  auto start = std::chrono::high_resolution_clock::now();

  auto foundCompositions = ifc.findCompositionsContaining(cfgTarget->getConfigurationName(), std::to_string(targetVersion));

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  std::cout << "Reverse lookup completed in " << duration.count() << "ms\n";

  BOOST_CHECK_EQUAL(foundCompositions.size(), 1);

  bool foundTarget = (foundCompositions.find(targetCompName) != foundCompositions.end());
  BOOST_CHECK_EQUAL(foundTarget, true);

  std::cout << "  Verified: Correct composition found efficiently\n";

  BOOST_CHECK_LT(duration.count(), 5000);
}

BOOST_AUTO_TEST_SUITE_END()
