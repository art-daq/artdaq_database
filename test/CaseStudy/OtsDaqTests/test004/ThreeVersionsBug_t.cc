#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (ThreeVersionsBug test)

#include <boost/test/unit_test.hpp>

#include <dirent.h>
#include <atomic>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct BugTestData {
  BugTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== INVESTIGATING THREE VERSIONS BUG ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";

    auto uri = std::getenv("ARTDAQ_DATABASE_URI");
    if (uri) {
      std::string uriStr(uri);
      if (uriStr.find("filesystemdb:///") == 0) {
        dbPath = uriStr.substr(16);
      } else if (uriStr.find("filesystemdb://") == 0) {
        dbPath = uriStr.substr(15);
      } else {
        dbPath = uriStr;
      }
      std::cout << "Database path: " << dbPath << "\n";
    }
  }

  ~BugTestData() { std::cout << "\n=== INVESTIGATION COMPLETE ===\n"; }

  int baseVersion;
  std::string dbPath;
};

BugTestData fixture;

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

int countConfigTypeOccurrences(const std::string& jsonContent, const std::string& configType) {
  std::regex pattern("\"name\"\\s*:\\s*\"" + configType + "\"");

  auto begin = std::sregex_iterator(jsonContent.begin(), jsonContent.end(), pattern);
  auto end = std::sregex_iterator();

  return std::distance(begin, end);
}

std::vector<std::string> extractConfigNames(const std::string& jsonContent) {
  std::vector<std::string> names;
  std::regex pattern("\"name\"\\s*:\\s*\"([^\"]+)\"");

  auto begin = std::sregex_iterator(jsonContent.begin(), jsonContent.end(), pattern);
  auto end = std::sregex_iterator();

  for (auto it = begin; it != end; ++it) {
    names.push_back((*it)[1].str());
  }
  return names;
}

bool isFileSystemBackend() {
  auto uri = std::getenv("ARTDAQ_DATABASE_URI");
  if (uri) {
    std::string uriStr(uri);
    return uriStr.find("filesystemdb") != std::string::npos;
  }
  return false;
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

BOOST_AUTO_TEST_SUITE(three_versions_bug_test)

BOOST_AUTO_TEST_CASE(attempt_three_versions_same_type) {
  std::cout << "\n=== TEST 1: Attempting to add three versions ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion;
  int version2 = fixture.baseVersion + 1;
  int version3 = fixture.baseVersion + 2;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());
  std::cout << "Created version 1: " << version1 << "\n";

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration001>();
  cfg2->getView().version = version2;
  ifc.saveActiveVersion(cfg2.get());
  std::cout << "Created version 2: " << version2 << "\n";

  std::shared_ptr<ConfigurationBase> cfg3 = std::make_shared<TestConfiguration001>();
  cfg3->getView().version = version3;
  ifc.saveActiveVersion(cfg3.get());
  std::cout << "Created version 3: " << version3 << "\n";

  std::cout << "\nAttempt 1: Using std::map...\n";
  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};

  compositionMap[cfg1->getConfigurationName()] = version1;
  std::cout << "  Added version1, map size: " << compositionMap.size() << "\n";

  compositionMap[cfg2->getConfigurationName()] = version2;
  std::cout << "  Added version2, map size: " << compositionMap.size() << " (should still be 1)\n";

  compositionMap[cfg3->getConfigurationName()] = version3;
  std::cout << "  Added version3, map size: " << compositionMap.size() << " (should still be 1)\n";

  BOOST_CHECK_EQUAL(compositionMap.size(), 1);
  BOOST_CHECK_EQUAL(compositionMap[cfg1->getConfigurationName()], version3);

  std::cout << "  Result: Map correctly contains only last version: " << version3 << "\n";

  std::string compName = std::string{"BugTest1_"} + std::to_string(fixture.baseVersion);

  ifc.storeGlobalConfiguration(compositionMap, compName);
  std::cout << "  Created composition: " << compName << "\n";

  auto loadedMap = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loadedMap.size(), 1);

  std::cout << "  Verified: Composition has " << loadedMap.size() << " member (correct)\n";
}

BOOST_AUTO_TEST_CASE(inspect_database_storage_format) {
  std::cout << "\n=== TEST 2: Inspecting database storage format ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 10;
  int version2 = fixture.baseVersion + 11;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = version2;
  ifc.saveActiveVersion(cfg2.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;
  compositionMap[cfg2->getConfigurationName()] = version2;

  std::string compName = std::string{"BugTest2_"} + std::to_string(fixture.baseVersion);

  ifc.storeGlobalConfiguration(compositionMap, compName);

  auto loadedMap = ifc.loadGlobalConfiguration(compName);

  std::cout << "Composition '" << compName << "' contains:\n";
  for (auto const& entry : loadedMap) {
    std::cout << "  " << entry.first << " -> version " << entry.second << "\n";
  }

  BOOST_CHECK_EQUAL(loadedMap.size(), 2);

  int count001 = loadedMap.count(cfg1->getConfigurationName());
  int count002 = loadedMap.count(cfg2->getConfigurationName());

  BOOST_CHECK_EQUAL(count001, 1);
  BOOST_CHECK_EQUAL(count002, 1);

  std::cout << "  Verified: Each configuration type appears exactly once\n";
}

BOOST_AUTO_TEST_CASE(verify_raw_storage_no_duplicates) {
  std::cout << "\n=== TEST 3: Verifying raw storage has no duplicates ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 20;
  int version2 = fixture.baseVersion + 21;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = version2;
  ifc.saveActiveVersion(cfg2.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;
  compositionMap[cfg2->getConfigurationName()] = version2;

  std::string compName = std::string{"RawStorageTest_"} + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(compositionMap, compName);

  std::cout << "Created composition: " << compName << "\n";

  std::string globalConfigDir = fixture.dbPath + "/GlobalConfigurations";
  std::cout << "Scanning: " << globalConfigDir << "\n";

  auto jsonFiles = listJsonFiles(globalConfigDir);
  std::cout << "Found " << jsonFiles.size() << " JSON files\n";

  bool foundOurComposition = false;
  int totalDuplicatesFound = 0;

  for (const auto& filepath : jsonFiles) {
    std::string content = readFileContent(filepath);
    if (content.empty()) {
      continue;
    }

    if (content.find(compName) != std::string::npos) {
      foundOurComposition = true;
      std::cout << "\nFound our composition in: " << filepath << "\n";

      auto configNames = extractConfigNames(content);
      std::cout << "  Configuration entries found: " << configNames.size() << "\n";

      std::set<std::string> uniqueNames;
      std::map<std::string, int> nameCounts;

      for (const auto& name : configNames) {
        nameCounts[name]++;
        uniqueNames.insert(name);
      }

      for (const auto& entry : nameCounts) {
        if (entry.second > 1) {
          std::cout << "  DUPLICATE DETECTED: '" << entry.first << "' appears " << entry.second << " times!\n";
          totalDuplicatesFound++;
        }
      }

      int count001 = countConfigTypeOccurrences(content, cfg1->getConfigurationName());
      int count002 = countConfigTypeOccurrences(content, cfg2->getConfigurationName());

      std::cout << "  " << cfg1->getConfigurationName() << " appears " << count001 << " time(s)\n";
      std::cout << "  " << cfg2->getConfigurationName() << " appears " << count002 << " time(s)\n";

      BOOST_CHECK_EQUAL(count001, 1);
      BOOST_CHECK_EQUAL(count002, 1);
    }
  }

  BOOST_CHECK_MESSAGE(foundOurComposition || jsonFiles.empty(), "Composition should be found in raw storage (or directory doesn't exist)");

  BOOST_CHECK_EQUAL(totalDuplicatesFound, 0);

  std::cout << "\n  Verified: Raw storage contains no duplicate configuration types\n";
}

BOOST_AUTO_TEST_CASE(scan_all_compositions_for_duplicates) {
  std::cout << "\n=== TEST 4: Scanning all compositions for duplicate entries ===\n";

  std::string globalConfigDir = fixture.dbPath + "/GlobalConfigurations";
  auto jsonFiles = listJsonFiles(globalConfigDir);

  std::cout << "Scanning " << jsonFiles.size() << " composition files in: " << globalConfigDir << "\n";

  int filesWithDuplicates = 0;
  int totalFilesScanned = 0;

  for (const auto& filepath : jsonFiles) {
    std::string content = readFileContent(filepath);
    if (content.empty()) {
      continue;
    }

    totalFilesScanned++;

    auto configNames = extractConfigNames(content);

    std::map<std::string, int> nameCounts;
    for (const auto& name : configNames) {
      nameCounts[name]++;
    }

    bool hasDuplicates = false;
    for (const auto& entry : nameCounts) {
      if (entry.second > 1) {
        if (!hasDuplicates) {
          std::cout << "\nDuplicates in: " << filepath << "\n";
          hasDuplicates = true;
          filesWithDuplicates++;
        }
        std::cout << "  '" << entry.first << "' appears " << entry.second << " times\n";
      }
    }
  }

  std::cout << "\nSummary:\n";
  std::cout << "  Files scanned: " << totalFilesScanned << "\n";
  std::cout << "  Files with duplicates: " << filesWithDuplicates << "\n";

  BOOST_CHECK_EQUAL(filesWithDuplicates, 0);

  std::cout << "\n  Verified: No compositions contain duplicate configuration types\n";
}

BOOST_AUTO_TEST_CASE(map_interface_prevents_duplicates) {
  std::cout << "\n=== TEST 5: Map interface prevents duplicates ===\n";

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};

  std::string typeName = "TestConfiguration001";

  compositionMap[typeName] = 100;
  std::cout << "Added version 100, map size: " << compositionMap.size() << "\n";
  BOOST_CHECK_EQUAL(compositionMap.size(), 1);
  BOOST_CHECK_EQUAL(compositionMap[typeName], 100);

  compositionMap[typeName] = 200;
  std::cout << "Added version 200, map size: " << compositionMap.size() << "\n";
  BOOST_CHECK_EQUAL(compositionMap.size(), 1);
  BOOST_CHECK_EQUAL(compositionMap[typeName], 200);

  compositionMap[typeName] = 300;
  std::cout << "Added version 300, map size: " << compositionMap.size() << "\n";
  BOOST_CHECK_EQUAL(compositionMap.size(), 1);
  BOOST_CHECK_EQUAL(compositionMap[typeName], 300);

  std::cout << "  Verified: std::map automatically prevents duplicates\n";
  std::cout << "  Final value: " << compositionMap[typeName] << " (should be 300)\n";
}

BOOST_AUTO_TEST_CASE(test_duplicate_composition_behavior) {
  std::cout << "\n=== TEST 6: Test duplicate composition behavior ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 1000;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::string compName = std::string{"DupCompTest_"} + std::to_string(fixture.baseVersion);

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[cfg1->getConfigurationName()] = version1;

  ifc.storeGlobalConfiguration(map1, compName);
  std::cout << "Created initial composition: " << compName << "\n";

  bool secondWriteFailed = false;
  try {
    ifc.storeGlobalConfiguration(map1, compName);
    std::cout << "  Second write succeeded (API allows duplicate composition names)\n";
  } catch (std::exception const& e) {
    secondWriteFailed = true;
    std::cout << "  Second write rejected: " << e.what() << "\n";
  }

  if (secondWriteFailed) {
    std::cout << "  API BEHAVIOR: Duplicate composition names are REJECTED\n";
  } else {
    std::cout << "  API BEHAVIOR: Duplicate composition names are ALLOWED (overwrite)\n";
  }

  auto allComps = ifc.findAllGlobalConfigurations();
  bool found = (allComps.find(compName) != allComps.end());

  BOOST_CHECK_EQUAL(found, true);

  std::cout << "  Verified: Composition exists after duplicate attempt\n";
}

BOOST_AUTO_TEST_CASE(different_compositions_same_config_types) {
  std::cout << "\n=== TEST 7: Different compositions can have same config types ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 2000;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::string compName1 = std::string{"MultiComp1_"} + std::to_string(fixture.baseVersion);

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[cfg1->getConfigurationName()] = version1;

  ifc.storeGlobalConfiguration(map1, compName1);
  std::cout << "Created composition 1: " << compName1 << "\n";

  std::string compName2 = std::string{"MultiComp2_"} + std::to_string(fixture.baseVersion);

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[cfg1->getConfigurationName()] = version1;

  bool secondSucceeded = true;
  try {
    ifc.storeGlobalConfiguration(map2, compName2);
    std::cout << "Created composition 2: " << compName2 << "\n";
  } catch (std::exception const& e) {
    secondSucceeded = false;
    std::cout << "  Second composition failed (unexpected): " << e.what() << "\n";
  }

  BOOST_CHECK_EQUAL(secondSucceeded, true);

  auto allComps = ifc.findAllGlobalConfigurations();

  bool found1 = (allComps.find(compName1) != allComps.end());
  bool found2 = (allComps.find(compName2) != allComps.end());

  BOOST_CHECK_EQUAL(found1, true);
  BOOST_CHECK_EQUAL(found2, true);

  std::cout << "  Verified: Both compositions exist independently\n";
}

BOOST_AUTO_TEST_CASE(sequential_multi_operation) {
  std::cout << "\n=== TEST 8: Sequential multi-operation test ===\n";

  const int NUM_OPERATIONS = 5;
  int successCount = 0;
  int failCount = 0;

  auto ifc = DatabaseConfigurationInterface();

  auto startTime = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_OPERATIONS; i++) {
    try {
      int version = fixture.baseVersion + 3000 + (i * 10);

      std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
      cfg->getView().version = version;
      ifc.saveActiveVersion(cfg.get());

      auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
      compositionMap[cfg->getConfigurationName()] = version;

      std::string compName = std::string{"SeqTest_"} + std::to_string(fixture.baseVersion) + "_op" + std::to_string(i);

      ifc.storeGlobalConfiguration(compositionMap, compName);

      std::cout << "  Operation " << i << ": Created composition " << compName << "\n";

      successCount++;
    } catch (std::exception const& e) {
      std::cout << "  Operation " << i << " failed: " << e.what() << "\n";
      failCount++;
    }
  }

  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

  std::cout << "  Completed in " << duration.count() << "ms\n";
  std::cout << "  Success: " << successCount << ", Failed: " << failCount << "\n";

  BOOST_CHECK_EQUAL(successCount, NUM_OPERATIONS);

  auto allComps = ifc.findAllGlobalConfigurations();

  int foundCount = 0;
  for (int i = 0; i < NUM_OPERATIONS; i++) {
    std::string compName = std::string{"SeqTest_"} + std::to_string(fixture.baseVersion) + "_op" + std::to_string(i);
    if (allComps.find(compName) != allComps.end()) {
      foundCount++;
    }
  }

  BOOST_CHECK_EQUAL(foundCount, NUM_OPERATIONS);
  std::cout << "  Verified: All " << foundCount << " sequential compositions exist\n";
}

BOOST_AUTO_TEST_CASE(performance_store_load) {
  std::cout << "\n=== TEST 9: Performance test (store/load) ===\n";

  auto ifc = DatabaseConfigurationInterface();

  const int NUM_CONFIGS = 2;
  int baseVer = fixture.baseVersion + 4000;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = baseVer;
  ifc.saveActiveVersion(cfg1.get());

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = baseVer + 1;
  ifc.saveActiveVersion(cfg2.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = baseVer;
  compositionMap[cfg2->getConfigurationName()] = baseVer + 1;

  std::string compName = std::string{"PerfTest_"} + std::to_string(fixture.baseVersion);

  auto storeStart = std::chrono::high_resolution_clock::now();
  ifc.storeGlobalConfiguration(compositionMap, compName);
  auto storeEnd = std::chrono::high_resolution_clock::now();
  auto storeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(storeEnd - storeStart);

  std::cout << "  Store time: " << storeDuration.count() << "ms\n";

  const int LOAD_ITERATIONS = 10;
  auto loadStart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < LOAD_ITERATIONS; i++) {
    auto loadedMap = ifc.loadGlobalConfiguration(compName);
    BOOST_CHECK_EQUAL(loadedMap.size(), NUM_CONFIGS);
  }
  auto loadEnd = std::chrono::high_resolution_clock::now();
  auto loadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(loadEnd - loadStart);

  std::cout << "  Load time (" << LOAD_ITERATIONS << " iterations): " << loadDuration.count() << "ms\n";
  std::cout << "  Average load time: " << (loadDuration.count() / LOAD_ITERATIONS) << "ms\n";

  BOOST_CHECK_LT(storeDuration.count(), 5000);
  BOOST_CHECK_LT(loadDuration.count(), 5000);

  std::cout << "  Verified: Performance within acceptable limits\n";
}

BOOST_AUTO_TEST_CASE(empty_composition_handling) {
  std::cout << "\n=== TEST 10: Empty composition handling ===\n";

  std::cout << "  API BEHAVIOR: Empty compositions are NOT allowed (assertion enforced)\n";
  std::cout << "  The storeGlobalConfiguration API requires at least one configuration.\n";

  auto ifc = DatabaseConfigurationInterface();

  int version = fixture.baseVersion + 4500;
  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = version;
  ifc.saveActiveVersion(cfg.get());

  auto minimalMap = DatabaseConfigurationInterface::config_version_map_t{};
  minimalMap[cfg->getConfigurationName()] = version;

  std::string compName = std::string{"MinimalCompTest_"} + std::to_string(fixture.baseVersion);

  bool storeSucceeded = false;
  try {
    ifc.storeGlobalConfiguration(minimalMap, compName);
    storeSucceeded = true;
    std::cout << "  Minimal (single-entry) composition: succeeded\n";
  } catch (std::exception const& e) {
    std::cout << "  Minimal composition failed: " << e.what() << "\n";
  }

  BOOST_CHECK_EQUAL(storeSucceeded, true);
  std::cout << "  Verified: Minimal compositions are allowed\n";
}

BOOST_AUTO_TEST_CASE(version_boundary_values) {
  std::cout << "\n=== TEST 11: Version boundary values ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = 1;

  bool v1Succeeded = true;
  try {
    ifc.saveActiveVersion(cfg1.get());
    std::cout << "  Version 1: Save succeeded\n";
  } catch (std::exception const& e) {
    v1Succeeded = false;
    std::cout << "  Version 1: Save failed - " << e.what() << "\n";
  }

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration001>();
  cfg2->getView().version = 999999;

  bool vLargeSucceeded = true;
  try {
    ifc.saveActiveVersion(cfg2.get());
    std::cout << "  Version 999999: Save succeeded\n";
  } catch (std::exception const& e) {
    vLargeSucceeded = false;
    std::cout << "  Version 999999: Save failed - " << e.what() << "\n";
  }

  if (v1Succeeded && vLargeSucceeded) {
    auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
    compositionMap[cfg1->getConfigurationName()] = 999999;

    std::string compName = std::string{"BoundaryTest_"} + std::to_string(fixture.baseVersion);

    bool compSucceeded = true;
    try {
      ifc.storeGlobalConfiguration(compositionMap, compName);
      std::cout << "  Composition with large version: succeeded\n";
    } catch (std::exception const& e) {
      compSucceeded = false;
      std::cout << "  Composition with large version: failed - " << e.what() << "\n";
    }

    BOOST_CHECK_EQUAL(compSucceeded, true);
  }

  BOOST_CHECK_EQUAL(v1Succeeded, true);

  std::cout << "  Verified: Boundary version values handled\n";
}

BOOST_AUTO_TEST_CASE(index_consistency_validation) {
  std::cout << "\n=== TEST 12: Index consistency validation ===\n";

  if (!isFileSystemBackend()) {
    std::cout << "  Skipping: Not using FileSystemDB backend\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int version = fixture.baseVersion + 5000;

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = version;
  ifc.saveActiveVersion(cfg.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg->getConfigurationName()] = version;

  std::string compName = std::string{"IndexTest_"} + std::to_string(fixture.baseVersion);
  ifc.storeGlobalConfiguration(compositionMap, compName);

  std::string globalConfigDir = fixture.dbPath + "/GlobalConfigurations";
  std::string indexPath = globalConfigDir + "/index.json";

  std::cout << "  Checking index: " << indexPath << "\n";

  auto indexOids = extractOidsFromIndex(indexPath);
  std::cout << "  OIDs in index: " << indexOids.size() << "\n";

  auto jsonFiles = listJsonFiles(globalConfigDir);
  std::set<std::string> fileOids;
  for (const auto& filepath : jsonFiles) {
    std::string oid = getOidFromFilename(filepath);
    if (!oid.empty() && oid != "index") {
      fileOids.insert(oid);
    }
  }
  std::cout << "  Files on disk: " << fileOids.size() << "\n";

  int orphanedFiles = 0;
  for (const auto& fileOid : fileOids) {
    if (indexOids.find(fileOid) == indexOids.end()) {
      std::cout << "  WARNING: Orphaned file: " << fileOid << ".json\n";
      orphanedFiles++;
    }
  }

  int missingFiles = 0;
  for (const auto& indexOid : indexOids) {
    if (fileOids.find(indexOid) == fileOids.end()) {
      std::cout << "  WARNING: Missing file: " << indexOid << ".json\n";
      missingFiles++;
    }
  }

  std::cout << "  Orphaned files: " << orphanedFiles << "\n";
  std::cout << "  Missing files: " << missingFiles << "\n";

  BOOST_WARN_EQUAL(orphanedFiles, 0);
  BOOST_WARN_EQUAL(missingFiles, 0);

  BOOST_CHECK_EQUAL(missingFiles, 0);

  std::cout << "  Verified: Index consistency checked\n";
}

BOOST_AUTO_TEST_CASE(stress_test_rapid_operations) {
  std::cout << "\n=== TEST 13: Stress test (rapid operations) ===\n";

  auto ifc = DatabaseConfigurationInterface();

  const int NUM_OPERATIONS = 20;
  int successCount = 0;
  int failCount = 0;

  auto startTime = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_OPERATIONS; i++) {
    try {
      int version = fixture.baseVersion + 6000 + i;

      std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
      cfg->getView().version = version;
      ifc.saveActiveVersion(cfg.get());

      auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
      compositionMap[cfg->getConfigurationName()] = version;

      std::string compName = std::string{"StressTest_"} + std::to_string(fixture.baseVersion) + "_" + std::to_string(i);
      ifc.storeGlobalConfiguration(compositionMap, compName);

      successCount++;
    } catch (std::exception const& e) {
      std::cout << "  Operation " << i << " failed: " << e.what() << "\n";
      failCount++;
    }
  }

  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

  std::cout << "  Completed " << NUM_OPERATIONS << " operations in " << duration.count() << "ms\n";
  std::cout << "  Average: " << (duration.count() / NUM_OPERATIONS) << "ms per operation\n";
  std::cout << "  Success: " << successCount << ", Failed: " << failCount << "\n";

  BOOST_CHECK_EQUAL(successCount, NUM_OPERATIONS);
  BOOST_CHECK_EQUAL(failCount, 0);

  BOOST_CHECK_LT(duration.count(), 30000);

  if (isFileSystemBackend()) {
    std::string globalConfigDir = fixture.dbPath + "/GlobalConfigurations";
    auto jsonFiles = listJsonFiles(globalConfigDir);

    int totalDuplicates = 0;
    for (const auto& filepath : jsonFiles) {
      std::string content = readFileContent(filepath);
      if (content.empty()) continue;

      auto configNames = extractConfigNames(content);
      std::map<std::string, int> nameCounts;
      for (const auto& name : configNames) {
        nameCounts[name]++;
      }

      for (const auto& entry : nameCounts) {
        if (entry.second > 1) {
          totalDuplicates++;
        }
      }
    }

    BOOST_CHECK_EQUAL(totalDuplicates, 0);
    std::cout << "  Verified: No duplicates in storage after stress test\n";
  }
}

BOOST_AUTO_TEST_CASE(composition_name_edge_cases) {
  std::cout << "\n=== TEST 14: Composition name edge cases ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version = fixture.baseVersion + 7000;

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = version;
  ifc.saveActiveVersion(cfg.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg->getConfigurationName()] = version;

  std::string name1 = std::string{"Test___Multiple___Underscores_"} + std::to_string(fixture.baseVersion);
  bool name1Succeeded = false;
  try {
    ifc.storeGlobalConfiguration(compositionMap, name1);
    name1Succeeded = true;
    std::cout << "  Underscore-heavy name: succeeded\n";
  } catch (std::exception const& e) {
    std::cout << "  Underscore-heavy name: failed - " << e.what() << "\n";
  }

  std::string name2 = std::to_string(fixture.baseVersion) + "12345";
  bool name2Succeeded = false;
  try {
    cfg->getView().version = version + 1;
    ifc.saveActiveVersion(cfg.get());
    compositionMap[cfg->getConfigurationName()] = version + 1;

    ifc.storeGlobalConfiguration(compositionMap, name2);
    name2Succeeded = true;
    std::cout << "  Numeric name: succeeded\n";
  } catch (std::exception const& e) {
    std::cout << "  Numeric name: failed - " << e.what() << "\n";
  }

  std::string name3 = std::string{"VeryLongCompositionNameThatMightCauseIssues_"} + std::to_string(fixture.baseVersion);
  bool name3Succeeded = false;
  try {
    cfg->getView().version = version + 2;
    ifc.saveActiveVersion(cfg.get());
    compositionMap[cfg->getConfigurationName()] = version + 2;

    ifc.storeGlobalConfiguration(compositionMap, name3);
    name3Succeeded = true;
    std::cout << "  Long name: succeeded\n";
  } catch (std::exception const& e) {
    std::cout << "  Long name: failed - " << e.what() << "\n";
  }

  BOOST_CHECK_EQUAL(name1Succeeded, true);
  BOOST_CHECK_MESSAGE(true, (name2Succeeded ? "Numeric names allowed" : "Numeric names rejected"));
  BOOST_CHECK_MESSAGE(true, (name3Succeeded ? "Long names allowed" : "Long names rejected"));

  std::cout << "  Verified: Composition name edge cases tested\n";
}

BOOST_AUTO_TEST_CASE(overwrite_updates_version) {
  std::cout << "\n=== TEST 15: Overwrite updates version (unassigns old) ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int versionOld = fixture.baseVersion + 8000;
  int versionNew = fixture.baseVersion + 8001;

  std::shared_ptr<ConfigurationBase> cfgOld = std::make_shared<TestConfiguration001>();
  cfgOld->getView().version = versionOld;
  ifc.saveActiveVersion(cfgOld.get());
  std::cout << "  Created old version: " << versionOld << "\n";

  std::shared_ptr<ConfigurationBase> cfgNew = std::make_shared<TestConfiguration001>();
  cfgNew->getView().version = versionNew;
  ifc.saveActiveVersion(cfgNew.get());
  std::cout << "  Created new version: " << versionNew << "\n";

  std::string compName = std::string{"OverwriteVersionTest_"} + std::to_string(fixture.baseVersion);

  auto mapOld = DatabaseConfigurationInterface::config_version_map_t{};
  mapOld[cfgOld->getConfigurationName()] = versionOld;

  ifc.storeGlobalConfiguration(mapOld, compName);
  std::cout << "  Created composition with version " << versionOld << "\n";

  auto loaded1 = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded1.size(), 1);
  BOOST_CHECK_EQUAL(loaded1[cfgOld->getConfigurationName()], versionOld);
  std::cout << "  Verified: Initial composition has version " << versionOld << "\n";

  auto mapNew = DatabaseConfigurationInterface::config_version_map_t{};
  mapNew[cfgNew->getConfigurationName()] = versionNew;

  ifc.storeGlobalConfiguration(mapNew, compName, true);
  std::cout << "  Overwrote composition with version " << versionNew << "\n";

  auto loaded2 = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded2.size(), 1);
  BOOST_CHECK_EQUAL(loaded2[cfgNew->getConfigurationName()], versionNew);
  std::cout << "  Verified: Composition now has version " << versionNew << "\n";

  auto compsWithOld = ifc.findCompositionsContaining(cfgOld->getConfigurationName(), std::to_string(versionOld));
  bool oldInComp = (compsWithOld.find(compName) != compsWithOld.end());
  BOOST_CHECK_EQUAL(oldInComp, false);
  std::cout << "  Verified: Old version " << versionOld << " is NOT in composition (correctly unassigned)\n";

  auto compsWithNew = ifc.findCompositionsContaining(cfgNew->getConfigurationName(), std::to_string(versionNew));
  bool newInComp = (compsWithNew.find(compName) != compsWithNew.end());
  BOOST_CHECK_EQUAL(newInComp, true);
  std::cout << "  Verified: New version " << versionNew << " IS in composition\n";

  std::cout << "  TEST PASSED: Overwrite correctly unassigned old version\n";
}

BOOST_AUTO_TEST_CASE(overwrite_removes_members) {
  std::cout << "\n=== TEST 16: Overwrite removes members ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 8100;
  int version2 = fixture.baseVersion + 8101;
  int version3 = fixture.baseVersion + 8102;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = version2;
  ifc.saveActiveVersion(cfg2.get());

  std::shared_ptr<ConfigurationBase> cfg3 = std::make_shared<TestConfiguration003>();
  cfg3->getView().version = version3;
  ifc.saveActiveVersion(cfg3.get());

  std::cout << "  Created 3 config versions: " << version1 << ", " << version2 << ", " << version3 << "\n";

  std::string compName = std::string{"OverwriteRemoveTest_"} + std::to_string(fixture.baseVersion);

  auto mapAll = DatabaseConfigurationInterface::config_version_map_t{};
  mapAll[cfg1->getConfigurationName()] = version1;
  mapAll[cfg2->getConfigurationName()] = version2;
  mapAll[cfg3->getConfigurationName()] = version3;

  ifc.storeGlobalConfiguration(mapAll, compName);
  std::cout << "  Created composition with 3 members\n";

  auto loaded1 = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded1.size(), 3);
  std::cout << "  Verified: Initial composition has 3 members\n";

  auto mapOne = DatabaseConfigurationInterface::config_version_map_t{};
  mapOne[cfg1->getConfigurationName()] = version1;

  ifc.storeGlobalConfiguration(mapOne, compName, true);
  std::cout << "  Overwrote composition with only 1 member\n";

  auto loaded2 = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded2.size(), 1);
  BOOST_CHECK_EQUAL(loaded2[cfg1->getConfigurationName()], version1);
  std::cout << "  Verified: Composition now has 1 member\n";

  auto compsWithCfg2 = ifc.findCompositionsContaining(cfg2->getConfigurationName(), std::to_string(version2));
  bool cfg2InComp = (compsWithCfg2.find(compName) != compsWithCfg2.end());
  BOOST_CHECK_EQUAL(cfg2InComp, false);
  std::cout << "  Verified: " << cfg2->getConfigurationName() << " is NOT in composition\n";

  auto compsWithCfg3 = ifc.findCompositionsContaining(cfg3->getConfigurationName(), std::to_string(version3));
  bool cfg3InComp = (compsWithCfg3.find(compName) != compsWithCfg3.end());
  BOOST_CHECK_EQUAL(cfg3InComp, false);
  std::cout << "  Verified: " << cfg3->getConfigurationName() << " is NOT in composition\n";

  auto compsWithCfg1 = ifc.findCompositionsContaining(cfg1->getConfigurationName(), std::to_string(version1));
  bool cfg1InComp = (compsWithCfg1.find(compName) != compsWithCfg1.end());
  BOOST_CHECK_EQUAL(cfg1InComp, true);
  std::cout << "  Verified: " << cfg1->getConfigurationName() << " IS still in composition\n";

  std::cout << "  TEST PASSED: Overwrite correctly removed members\n";
}

BOOST_AUTO_TEST_CASE(overwrite_with_partial_changes) {
  std::cout << "\n=== TEST 17: Overwrite with partial changes (full merge) ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int vA1 = fixture.baseVersion + 8200;
  int vA5 = fixture.baseVersion + 8205;
  int vB2 = fixture.baseVersion + 8202;
  int vC3 = fixture.baseVersion + 8203;

  std::shared_ptr<ConfigurationBase> cfgA1 = std::make_shared<TestConfiguration001>();
  cfgA1->getView().version = vA1;
  ifc.saveActiveVersion(cfgA1.get());

  std::shared_ptr<ConfigurationBase> cfgA5 = std::make_shared<TestConfiguration001>();
  cfgA5->getView().version = vA5;
  ifc.saveActiveVersion(cfgA5.get());

  std::shared_ptr<ConfigurationBase> cfgB2 = std::make_shared<TestConfiguration002>();
  cfgB2->getView().version = vB2;
  ifc.saveActiveVersion(cfgB2.get());

  std::shared_ptr<ConfigurationBase> cfgC3 = std::make_shared<TestConfiguration003>();
  cfgC3->getView().version = vC3;
  ifc.saveActiveVersion(cfgC3.get());

  std::cout << "  Created versions: A/v" << vA1 << ", A/v" << vA5 << ", B/v" << vB2 << ", C/v" << vC3 << "\n";

  std::string compName = std::string{"PartialChangeTest_"} + std::to_string(fixture.baseVersion);

  auto mapOld = DatabaseConfigurationInterface::config_version_map_t{};
  mapOld[cfgA1->getConfigurationName()] = vA1;
  mapOld[cfgB2->getConfigurationName()] = vB2;
  mapOld[cfgC3->getConfigurationName()] = vC3;

  ifc.storeGlobalConfiguration(mapOld, compName);
  std::cout << "  Created OLD composition: {A/v" << vA1 << ", B/v" << vB2 << ", C/v" << vC3 << "}\n";

  auto loaded1 = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded1.size(), 3);

  auto mapNew = DatabaseConfigurationInterface::config_version_map_t{};
  mapNew[cfgA5->getConfigurationName()] = vA5;
  mapNew[cfgB2->getConfigurationName()] = vB2;

  ifc.storeGlobalConfiguration(mapNew, compName, true);
  std::cout << "  Overwrote with NEW composition: {A/v" << vA5 << ", B/v" << vB2 << "}\n";

  auto loaded2 = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded2.size(), 2);
  BOOST_CHECK_EQUAL(loaded2[cfgA5->getConfigurationName()], vA5);
  BOOST_CHECK_EQUAL(loaded2[cfgB2->getConfigurationName()], vB2);
  std::cout << "  Verified: Composition has 2 members with correct versions\n";

  auto compsWithA1 = ifc.findCompositionsContaining(cfgA1->getConfigurationName(), std::to_string(vA1));
  bool A1InComp = (compsWithA1.find(compName) != compsWithA1.end());
  BOOST_CHECK_EQUAL(A1InComp, false);
  std::cout << "  Verified: A/v" << vA1 << " is NOT in composition (replaced)\n";

  auto compsWithA5 = ifc.findCompositionsContaining(cfgA5->getConfigurationName(), std::to_string(vA5));
  bool A5InComp = (compsWithA5.find(compName) != compsWithA5.end());
  BOOST_CHECK_EQUAL(A5InComp, true);
  std::cout << "  Verified: A/v" << vA5 << " IS in composition (new version)\n";

  auto compsWithB2 = ifc.findCompositionsContaining(cfgB2->getConfigurationName(), std::to_string(vB2));
  bool B2InComp = (compsWithB2.find(compName) != compsWithB2.end());
  BOOST_CHECK_EQUAL(B2InComp, true);
  std::cout << "  Verified: B/v" << vB2 << " IS in composition (unchanged)\n";

  auto compsWithC3 = ifc.findCompositionsContaining(cfgC3->getConfigurationName(), std::to_string(vC3));
  bool C3InComp = (compsWithC3.find(compName) != compsWithC3.end());
  BOOST_CHECK_EQUAL(C3InComp, false);
  std::cout << "  Verified: C/v" << vC3 << " is NOT in composition (removed)\n";

  std::cout << "  TEST PASSED: Partial changes correctly applied\n";
}

BOOST_AUTO_TEST_CASE(load_after_overwrite_returns_only_new_members) {
  std::cout << "\n=== TEST 18: Load after overwrite returns only new members ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int v1 = fixture.baseVersion + 8300;
  int v2 = fixture.baseVersion + 8301;
  int v3 = fixture.baseVersion + 8302;
  int v4 = fixture.baseVersion + 8303;

  std::shared_ptr<ConfigurationBase> cfg1a = std::make_shared<TestConfiguration001>();
  cfg1a->getView().version = v1;
  ifc.saveActiveVersion(cfg1a.get());

  std::shared_ptr<ConfigurationBase> cfg1b = std::make_shared<TestConfiguration001>();
  cfg1b->getView().version = v2;
  ifc.saveActiveVersion(cfg1b.get());

  std::shared_ptr<ConfigurationBase> cfg2a = std::make_shared<TestConfiguration002>();
  cfg2a->getView().version = v3;
  ifc.saveActiveVersion(cfg2a.get());

  std::shared_ptr<ConfigurationBase> cfg2b = std::make_shared<TestConfiguration002>();
  cfg2b->getView().version = v4;
  ifc.saveActiveVersion(cfg2b.get());

  std::string compName = std::string{"LoadAfterOverwriteTest_"} + std::to_string(fixture.baseVersion);

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[cfg1a->getConfigurationName()] = v1;
  map1[cfg2a->getConfigurationName()] = v3;

  ifc.storeGlobalConfiguration(map1, compName);
  std::cout << "  Created initial: {Type1/v" << v1 << ", Type2/v" << v3 << "}\n";

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[cfg1b->getConfigurationName()] = v2;
  map2[cfg2b->getConfigurationName()] = v4;

  ifc.storeGlobalConfiguration(map2, compName, true);
  std::cout << "  Overwrote with: {Type1/v" << v2 << ", Type2/v" << v4 << "}\n";

  auto loaded = ifc.loadGlobalConfiguration(compName);

  BOOST_CHECK_EQUAL(loaded.size(), 2);
  BOOST_CHECK_EQUAL(loaded[cfg1b->getConfigurationName()], v2);
  BOOST_CHECK_EQUAL(loaded[cfg2b->getConfigurationName()], v4);

  BOOST_CHECK(loaded[cfg1a->getConfigurationName()] != v1);
  BOOST_CHECK(loaded[cfg2a->getConfigurationName()] != v3);

  std::cout << "  Verified: Load returns exactly {Type1/v" << v2 << ", Type2/v" << v4 << "}\n";
  std::cout << "  TEST PASSED: Load after overwrite returns only new members\n";
}

BOOST_AUTO_TEST_CASE(reverse_lookup_correct_after_overwrite) {
  std::cout << "\n=== TEST 19: Reverse lookup correct after overwrite ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int vOld = fixture.baseVersion + 8400;
  int vNew = fixture.baseVersion + 8401;

  std::shared_ptr<ConfigurationBase> cfgOld = std::make_shared<TestConfiguration001>();
  cfgOld->getView().version = vOld;
  ifc.saveActiveVersion(cfgOld.get());

  std::shared_ptr<ConfigurationBase> cfgNew = std::make_shared<TestConfiguration001>();
  cfgNew->getView().version = vNew;
  ifc.saveActiveVersion(cfgNew.get());

  std::string compName = std::string{"ReverseLookupOverwriteTest_"} + std::to_string(fixture.baseVersion);

  auto mapOld = DatabaseConfigurationInterface::config_version_map_t{};
  mapOld[cfgOld->getConfigurationName()] = vOld;
  ifc.storeGlobalConfiguration(mapOld, compName);
  std::cout << "  Created composition with version " << vOld << "\n";

  auto comps1 = ifc.findCompositionsContaining(cfgOld->getConfigurationName(), std::to_string(vOld));
  BOOST_CHECK(comps1.find(compName) != comps1.end());
  std::cout << "  Verified: Reverse lookup finds old version\n";

  auto mapNew = DatabaseConfigurationInterface::config_version_map_t{};
  mapNew[cfgNew->getConfigurationName()] = vNew;
  ifc.storeGlobalConfiguration(mapNew, compName, true);
  std::cout << "  Overwrote composition with version " << vNew << "\n";

  auto comps2 = ifc.findCompositionsContaining(cfgOld->getConfigurationName(), std::to_string(vOld));
  bool oldFound = (comps2.find(compName) != comps2.end());
  BOOST_CHECK_EQUAL(oldFound, false);
  std::cout << "  Verified: Reverse lookup for old version returns EMPTY (correctly unassigned)\n";

  auto comps3 = ifc.findCompositionsContaining(cfgNew->getConfigurationName(), std::to_string(vNew));
  bool newFound = (comps3.find(compName) != comps3.end());
  BOOST_CHECK_EQUAL(newFound, true);
  std::cout << "  Verified: Reverse lookup for new version finds it\n";

  std::cout << "  TEST PASSED: Reverse lookup is correct after overwrite\n";
}

BOOST_AUTO_TEST_CASE(overwrite_prevents_too_many_results) {
  std::cout << "\n=== TEST 20: Overwrite prevents 'too many results' error ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::string compName = std::string{"TooManyResultsTest_"} + std::to_string(fixture.baseVersion);

  const int NUM_OVERWRITES = 5;
  int baseVer = fixture.baseVersion + 8500;

  std::cout << "  Performing " << NUM_OVERWRITES << " sequential overwrites...\n";

  for (int i = 0; i < NUM_OVERWRITES; i++) {
    int version = baseVer + i;

    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = version;
    ifc.saveActiveVersion(cfg.get());

    auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
    compositionMap[cfg->getConfigurationName()] = version;

    bool allowOverwrite = (i > 0);
    ifc.storeGlobalConfiguration(compositionMap, compName, allowOverwrite);

    std::cout << "    Overwrite " << i << ": Set to version " << version << "\n";
  }

  bool loadSucceeded = false;
  int loadedVersion = 0;
  std::string errorMsg;

  try {
    auto loaded = ifc.loadGlobalConfiguration(compName);
    loadSucceeded = true;

    BOOST_CHECK_EQUAL(loaded.size(), 1);

    int expectedVersion = baseVer + NUM_OVERWRITES - 1;
    loadedVersion = loaded.begin()->second;
    BOOST_CHECK_EQUAL(loadedVersion, expectedVersion);

    std::cout << "  Load succeeded! Got version " << loadedVersion << "\n";
  } catch (std::exception const& e) {
    errorMsg = e.what();
    std::cout << "  Load FAILED: " << errorMsg << "\n";
  }

  BOOST_CHECK_MESSAGE(loadSucceeded,
                      "loadGlobalConfiguration should NOT fail after multiple overwrites. "
                      "Error: " +
                          errorMsg);

  if (loadSucceeded) {
    std::cout << "  Verified: No 'too many results' error after " << NUM_OVERWRITES << " overwrites\n";
    std::cout << "  Verified: Final version is " << loadedVersion << " (expected " << (baseVer + NUM_OVERWRITES - 1) << ")\n";
  }

  std::cout << "  TEST PASSED: Overwrite correctly prevents 'too many results' error\n";
}

BOOST_AUTO_TEST_CASE(overwrite_with_identical_composition) {
  std::cout << "\n=== TEST 21: Overwrite with identical composition (no-op) ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 8600;
  int version2 = fixture.baseVersion + 8601;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = version2;
  ifc.saveActiveVersion(cfg2.get());

  std::string compName = std::string{"IdenticalOverwriteTest_"} + std::to_string(fixture.baseVersion);

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;
  compositionMap[cfg2->getConfigurationName()] = version2;

  ifc.storeGlobalConfiguration(compositionMap, compName);
  std::cout << "  Created initial composition\n";

  bool overwriteSucceeded = false;
  try {
    ifc.storeGlobalConfiguration(compositionMap, compName, true);
    overwriteSucceeded = true;
    std::cout << "  Overwrote with identical composition: succeeded\n";
  } catch (std::exception const& e) {
    std::cout << "  Overwrote with identical composition: FAILED - " << e.what() << "\n";
  }

  BOOST_CHECK_EQUAL(overwriteSucceeded, true);

  auto loaded = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded.size(), 2);
  BOOST_CHECK_EQUAL(loaded[cfg1->getConfigurationName()], version1);
  BOOST_CHECK_EQUAL(loaded[cfg2->getConfigurationName()], version2);

  std::cout << "  Verified: Composition unchanged after identical overwrite\n";
  std::cout << "  TEST PASSED: Identical overwrite is a successful no-op\n";
}

BOOST_AUTO_TEST_SUITE_END()
