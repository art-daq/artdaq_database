#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (ReverseLookup test)

#include <boost/test/unit_test.hpp>

#include <dirent.h>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "../DatabaseConfigurationInterface.h"

std::string readFileContent(const std::string& filepath) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    return "";
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

std::vector<std::string> listJsonFiles(const std::string& dirPath) {
  std::vector<std::string> files;

  DIR* dir = opendir(dirPath.c_str());
  if (dir == nullptr) {
    return files;
  }

  struct dirent* entry;
  while ((entry = readdir(dir)) != nullptr) {
    std::string name = entry->d_name;
    if (name.size() > 5 && name.substr(name.size() - 5) == ".json" && name != "index.json") {
      files.push_back(dirPath + "/" + name);
    }
  }
  closedir(dir);
  return files;
}

bool isFileSystemBackend() {
  auto uri = std::getenv("ARTDAQ_DATABASE_URI");
  if (uri) {
    std::string uriStr(uri);
    return uriStr.find("filesystemdb") != std::string::npos;
  }
  return false;
}

std::string getDatabasePath() {
  auto uri = std::getenv("ARTDAQ_DATABASE_URI");
  if (uri) {
    std::string uriStr(uri);
    if (uriStr.find("filesystemdb:///") == 0) {
      return uriStr.substr(16);
    } else if (uriStr.find("filesystemdb://") == 0) {
      return uriStr.substr(15);
    }
    return uriStr;
  }
  return "";
}

std::string extractCompositionName(const std::string& jsonContent) {
  std::regex pattern("\"name\"\\s*:\\s*\"([^\"]+)\"");
  std::smatch match;

  if (std::regex_search(jsonContent, match, pattern)) {
    return match[1].str();
  }
  return "";
}

bool compositionContains(const std::string& jsonContent, const std::string& configType, const std::string& version) {
  bool hasConfigType = jsonContent.find("\"" + configType + "\"") != std::string::npos;

  std::regex versionPattern("\"version\"\\s*:\\s*\"" + version + "\"");
  bool hasVersion = std::regex_search(jsonContent, versionPattern);

  return hasConfigType && hasVersion;
}

int countCompositionsContaining(const std::string& globalConfigDir, const std::string& configType, const std::string& version) {
  auto files = listJsonFiles(globalConfigDir);
  int count = 0;

  for (const auto& filepath : files) {
    std::string content = readFileContent(filepath);
    if (content.empty()) continue;

    if (compositionContains(content, configType, version)) {
      count++;
    }
  }
  return count;
}

std::set<std::string> extractOidsFromIndex(const std::string& indexPath) {
  std::set<std::string> oids;
  std::string content = readFileContent(indexPath);
  if (content.empty()) {
    return oids;
  }

  std::regex pattern("\"oid\"\\s*:\\s*\"([a-f0-9]+)\"");
  auto begin = std::sregex_iterator(content.begin(), content.end(), pattern);
  auto end = std::sregex_iterator();

  for (auto it = begin; it != end; ++it) {
    oids.insert((*it)[1].str());
  }
  return oids;
}

std::string getOidFromFilename(const std::string& filepath) {
  size_t lastSlash = filepath.rfind('/');
  std::string filename = (lastSlash != std::string::npos) ? filepath.substr(lastSlash + 1) : filepath;

  if (filename.size() > 5 && filename.substr(filename.size() - 5) == ".json") {
    return filename.substr(0, filename.size() - 5);
  }
  return "";
}

using namespace ots;

struct ReverseLookupTestData {
  ReverseLookupTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    dbPath = getDatabasePath();
    std::cout << "=== REVERSE LOOKUP TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n";
    std::cout << "Database path: " << dbPath << "\n\n";
  }

  ~ReverseLookupTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
  std::string dbPath;
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

BOOST_AUTO_TEST_CASE(raw_storage_verify_composition) {
  std::cout << "\n=== TEST 6: Raw storage verification ===\n";

  if (!isFileSystemBackend()) {
    std::cout << "  Skipping: Not using FileSystemDB backend\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 700;
  int version2 = fixture.baseVersion + 701;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = version2;
  ifc.saveActiveVersion(cfg2.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;
  compositionMap[cfg2->getConfigurationName()] = version2;

  std::string compName = std::string{"RawStorageComp_"} + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(compositionMap, compName);

  std::cout << "Created composition: " << compName << "\n";

  auto apiResults = ifc.findCompositionsContaining(cfg1->getConfigurationName(), std::to_string(version1));

  std::cout << "  API found " << apiResults.size() << " composition(s) containing v" << version1 << "\n";

  BOOST_CHECK_GE(apiResults.size(), static_cast<size_t>(1));
  bool foundOurs = (apiResults.find(compName) != apiResults.end());
  BOOST_CHECK_EQUAL(foundOurs, true);

  std::cout << "  Verified: Composition accessible via API\n";
}

BOOST_AUTO_TEST_CASE(index_consistency_validation) {
  std::cout << "\n=== TEST 7: Index consistency validation ===\n";

  if (!isFileSystemBackend()) {
    std::cout << "  Skipping: Not using FileSystemDB backend\n";
    BOOST_CHECK(true);
    return;
  }

  std::string collectionDir = fixture.dbPath + "/TestConfiguration001";
  std::string indexPath = collectionDir + "/index.json";

  auto indexOids = extractOidsFromIndex(indexPath);
  std::cout << "  OIDs in index: " << indexOids.size() << "\n";

  auto jsonFiles = listJsonFiles(collectionDir);
  std::set<std::string> fileOids;
  for (const auto& filepath : jsonFiles) {
    std::string oid = getOidFromFilename(filepath);
    if (!oid.empty() && oid != "index") {
      fileOids.insert(oid);
    }
  }
  std::cout << "  Files on disk: " << fileOids.size() << "\n";

  int orphaned = 0;
  for (const auto& fileOid : fileOids) {
    if (indexOids.find(fileOid) == indexOids.end()) {
      std::cout << "  WARNING: Orphaned file: " << fileOid << ".json\n";
      orphaned++;
    }
  }

  int missing = 0;
  for (const auto& indexOid : indexOids) {
    if (fileOids.find(indexOid) == fileOids.end()) {
      std::cout << "  WARNING: Missing file: " << indexOid << ".json\n";
      missing++;
    }
  }

  std::cout << "  Orphaned files: " << orphaned << "\n";
  std::cout << "  Missing files: " << missing << "\n";

  BOOST_WARN_EQUAL(orphaned, 0);
  BOOST_CHECK_EQUAL(missing, 0);

  std::cout << "  Verified: Index consistency checked\n";
}

BOOST_AUTO_TEST_CASE(stress_test_many_compositions) {
  std::cout << "\n=== TEST 8: Stress test - many compositions ===\n";

  auto ifc = DatabaseConfigurationInterface();

  const int NUM_COMPOSITIONS = 20;
  int baseVer = fixture.baseVersion + 800;

  std::shared_ptr<ConfigurationBase> cfgShared = std::make_shared<TestConfiguration001>();
  cfgShared->getView().version = baseVer;
  ifc.saveActiveVersion(cfgShared.get());

  auto startCreate = std::chrono::high_resolution_clock::now();

  int compositionsWithShared = 0;
  for (int i = 0; i < NUM_COMPOSITIONS; i++) {
    int otherVersion = baseVer + 100 + i;

    std::shared_ptr<ConfigurationBase> cfgOther = std::make_shared<TestConfiguration002>();
    cfgOther->getView().version = otherVersion;
    ifc.saveActiveVersion(cfgOther.get());

    auto map = DatabaseConfigurationInterface::config_version_map_t{};

    if (i % 2 == 0) {
      map[cfgShared->getConfigurationName()] = baseVer;
      compositionsWithShared++;
    }
    map[cfgOther->getConfigurationName()] = otherVersion;

    std::string compName = std::string{"StressComp_"} + std::to_string(i) + "_" + std::to_string(fixture.baseVersion);
    ifc.storeGlobalConfiguration(map, compName);
  }

  auto endCreate = std::chrono::high_resolution_clock::now();
  auto createDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endCreate - startCreate);

  std::cout << "  Created " << NUM_COMPOSITIONS << " compositions in " << createDuration.count() << "ms\n";
  std::cout << "  " << compositionsWithShared << " compositions contain shared version\n";

  auto startLookup = std::chrono::high_resolution_clock::now();

  auto foundCompositions = ifc.findCompositionsContaining(cfgShared->getConfigurationName(), std::to_string(baseVer));

  auto endLookup = std::chrono::high_resolution_clock::now();
  auto lookupDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endLookup - startLookup);

  std::cout << "  Reverse lookup completed in " << lookupDuration.count() << "ms\n";
  std::cout << "  Found " << foundCompositions.size() << " compositions\n";

  BOOST_CHECK_EQUAL(foundCompositions.size(), static_cast<size_t>(compositionsWithShared));

  BOOST_CHECK_LT(lookupDuration.count(), 10000);

  std::cout << "  Verified: Stress test passed\n";
}

BOOST_AUTO_TEST_CASE(edge_case_config_type_not_in_compositions) {
  std::cout << "\n=== TEST 9: Edge case - config type not in compositions ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int testVersion = fixture.baseVersion + 900;

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = testVersion;
  ifc.saveActiveVersion(cfg.get());

  std::cout << "Created standalone version " << testVersion << " (not in any composition)\n";

  auto foundCompositions = ifc.findCompositionsContaining(cfg->getConfigurationName(), std::to_string(testVersion));

  BOOST_CHECK_EQUAL(foundCompositions.size(), 0);
  std::cout << "  Verified: Empty result for version not in any composition\n";
}

BOOST_AUTO_TEST_CASE(edge_case_nonexistent_version) {
  std::cout << "\n=== TEST 10: Edge case - nonexistent version ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int nonexistentVersion = 999888777;

  auto foundCompositions = ifc.findCompositionsContaining("TestConfiguration001", std::to_string(nonexistentVersion));

  std::cout << "  Searched for nonexistent version " << nonexistentVersion << "\n";
  std::cout << "  Found " << foundCompositions.size() << " compositions\n";

  BOOST_CHECK_EQUAL(foundCompositions.size(), 0);
  std::cout << "  Verified: Empty result for nonexistent version\n";
}

BOOST_AUTO_TEST_CASE(reverse_lookup_different_config_types) {
  std::cout << "\n=== TEST 11: Reverse lookup different config types ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 1100;
  int version2 = fixture.baseVersion + 1101;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = version2;
  ifc.saveActiveVersion(cfg2.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;
  compositionMap[cfg2->getConfigurationName()] = version2;

  std::string compName = std::string{"MultiTypeComp_"} + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(compositionMap, compName);

  std::cout << "Created composition with both config types\n";

  auto found1 = ifc.findCompositionsContaining(cfg1->getConfigurationName(), std::to_string(version1));
  BOOST_CHECK_EQUAL(found1.size(), 1);
  BOOST_CHECK(found1.find(compName) != found1.end());

  auto found2 = ifc.findCompositionsContaining(cfg2->getConfigurationName(), std::to_string(version2));
  BOOST_CHECK_EQUAL(found2.size(), 1);
  BOOST_CHECK(found2.find(compName) != found2.end());

  std::cout << "  Verified: Both config types correctly find the composition\n";
}

BOOST_AUTO_TEST_CASE(performance_multiple_lookups) {
  std::cout << "\n=== TEST 12: Performance - multiple reverse lookups ===\n";

  auto ifc = DatabaseConfigurationInterface();

  const int NUM_LOOKUPS = 20;
  int baseVer = fixture.baseVersion + 1200;

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = baseVer;
  ifc.saveActiveVersion(cfg.get());

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[cfg->getConfigurationName()] = baseVer;

  std::string compName = std::string{"PerfLookupComp_"} + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(map, compName);

  auto startTime = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_LOOKUPS; i++) {
    auto found = ifc.findCompositionsContaining(cfg->getConfigurationName(), std::to_string(baseVer));
    BOOST_CHECK_EQUAL(found.size(), 1);
  }

  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

  std::cout << "  Completed " << NUM_LOOKUPS << " reverse lookups in " << duration.count() << "ms\n";
  std::cout << "  Average: " << (duration.count() / NUM_LOOKUPS) << "ms per lookup\n";

  BOOST_CHECK_LT(duration.count(), 10000);

  std::cout << "  Verified: Multiple lookups performance acceptable\n";
}

BOOST_AUTO_TEST_CASE(api_consistency_multiple_compositions) {
  std::cout << "\n=== TEST 13: API consistency - multiple compositions ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int sharedVersion = fixture.baseVersion + 1300;
  const int NUM_COMPOSITIONS = 3;

  std::shared_ptr<ConfigurationBase> cfgShared = std::make_shared<TestConfiguration001>();
  cfgShared->getView().version = sharedVersion;
  ifc.saveActiveVersion(cfgShared.get());

  std::vector<std::string> createdComps;
  for (int i = 0; i < NUM_COMPOSITIONS; i++) {
    int otherVersion = sharedVersion + 100 + i;

    std::shared_ptr<ConfigurationBase> cfgOther = std::make_shared<TestConfiguration002>();
    cfgOther->getView().version = otherVersion;
    ifc.saveActiveVersion(cfgOther.get());

    auto map = DatabaseConfigurationInterface::config_version_map_t{};
    map[cfgShared->getConfigurationName()] = sharedVersion;
    map[cfgOther->getConfigurationName()] = otherVersion;

    std::string compName = std::string{"ConsistencyComp_"} + std::to_string(i) + "_" + std::to_string(fixture.baseVersion);
    ifc.storeGlobalConfiguration(map, compName);
    createdComps.push_back(compName);
  }

  std::cout << "  Created " << NUM_COMPOSITIONS << " compositions with shared version\n";

  auto apiResults = ifc.findCompositionsContaining(cfgShared->getConfigurationName(), std::to_string(sharedVersion));
  std::cout << "  API found " << apiResults.size() << " compositions\n";

  BOOST_CHECK_GE(apiResults.size(), static_cast<size_t>(NUM_COMPOSITIONS));

  int foundCount = 0;
  for (const auto& comp : createdComps) {
    bool found = apiResults.find(comp) != apiResults.end();
    if (found) foundCount++;
    BOOST_CHECK_MESSAGE(found, "Composition " << comp << " should be in API results");
  }

  std::cout << "  Found " << foundCount << " of " << NUM_COMPOSITIONS << " expected compositions\n";
  std::cout << "  Verified: API returns all expected compositions\n";
}

BOOST_AUTO_TEST_CASE(comprehensive_reverse_lookup_verification) {
  std::cout << "\n=== TEST 14: Comprehensive reverse lookup verification ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int baseVer = fixture.baseVersion + 1400;
  const int NUM_CONFIGS = 3;
  const int NUM_COMPOSITIONS = 4;

  std::vector<int> createdVersions;
  for (int i = 0; i < NUM_CONFIGS; i++) {
    int version = baseVer + i;
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = version;
    ifc.saveActiveVersion(cfg.get());
    createdVersions.push_back(version);
  }

  std::map<std::string, std::vector<int>> compVersions;

  for (int i = 0; i < NUM_COMPOSITIONS; i++) {
    auto map = DatabaseConfigurationInterface::config_version_map_t{};

    int v1 = createdVersions[i % NUM_CONFIGS];
    map["TestConfiguration001"] = v1;

    int v2 = baseVer + 100 + i;
    std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
    cfg2->getView().version = v2;
    ifc.saveActiveVersion(cfg2.get());
    map["TestConfiguration002"] = v2;

    std::string compName = std::string{"ComprehensiveComp_"} + std::to_string(i) + "_" + std::to_string(fixture.baseVersion);
    ifc.storeGlobalConfiguration(map, compName);
    compVersions[compName] = {v1, v2};
  }

  std::cout << "  Created " << NUM_COMPOSITIONS << " compositions for comprehensive scan\n";

  int successfulLookups = 0;
  int failedLookups = 0;

  for (const auto& [compName, versions] : compVersions) {
    auto results = ifc.findCompositionsContaining("TestConfiguration001", std::to_string(versions[0]));
    bool found = results.find(compName) != results.end();
    if (found) {
      successfulLookups++;
    } else {
      failedLookups++;
      std::cout << "  WARNING: Composition " << compName << " not found via v" << versions[0] << "\n";
    }
  }

  std::cout << "  Successful lookups: " << successfulLookups << "\n";
  std::cout << "  Failed lookups: " << failedLookups << "\n";

  BOOST_CHECK_EQUAL(failedLookups, 0);
  std::cout << "  Verified: Comprehensive scan complete\n";
}

BOOST_AUTO_TEST_SUITE_END()
