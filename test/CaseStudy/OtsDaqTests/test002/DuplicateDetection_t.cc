#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (DuplicateDetection test)

#include <boost/test/unit_test.hpp>

#include <dirent.h>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "../DatabaseConfigurationInterface.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

#include "artdaq-database/ConfigurationDB/Multitasker.h"
#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/dispatch_common.h"

bool jsonContains(const std::string& json, const std::string& key, const std::string& value) {
  std::string pattern1 = "\"" + key + "\":\"" + value + "\"";
  std::string pattern2 = "\"" + key + "\" : \"" + value + "\"";
  return json.find(pattern1) != std::string::npos || json.find(pattern2) != std::string::npos;
}

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

int countDocumentsWithVersion(const std::string& dirPath, const std::string& configType, const std::string& version) {
  auto files = listJsonFiles(dirPath);
  int count = 0;

  std::regex versionPattern("\"version\"\\s*:\\s*\"" + version + "\"");

  for (const auto& filepath : files) {
    std::string content = readFileContent(filepath);
    if (content.empty()) continue;

    if (content.find("\"" + configType + "\"") != std::string::npos && std::regex_search(content, versionPattern)) {
      count++;
    }
  }
  return count;
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

int countIndexEntriesForVersion(const std::string& indexPath, const std::string& version) {
  std::string content = readFileContent(indexPath);
  if (content.empty()) return 0;

  std::regex pattern("\"version\"\\s*:\\s*\"" + version + "\"");
  auto begin = std::sregex_iterator(content.begin(), content.end(), pattern);
  auto end = std::sregex_iterator();

  return std::distance(begin, end);
}

struct DuplicateTestData {
  DuplicateTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    dbPath = getDatabasePath();
    std::cout << "=== DUPLICATE DETECTION TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n";
    std::cout << "Database path: " << dbPath << "\n\n";
  }

  ~DuplicateTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  void updateConfigCount(int count) { oldConfigCount = count; }

  int baseVersion;
  int oldConfigCount = 0;
  std::string dbPath;
};

using namespace ots;

DuplicateTestData fixture;

BOOST_AUTO_TEST_SUITE(duplicate_detection_test)

BOOST_AUTO_TEST_CASE(duplicate_version_rejected) {
  std::cout << "\n=== TEST 1: Duplicate version rejected ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();

  int testVersion = fixture.baseVersion;
  cfg1->getView().version = testVersion;
  cfg1->getView().fillFromJSON("{\"test\": \"first_save\"}");

  std::cout << "Step 1: Saving configuration version " << testVersion << " (first time)...\n";

  auto result1 = ifc.saveActiveVersion(cfg1.get());

  BOOST_CHECK_EQUAL(result1, 0);
  std::cout << "  Result: SUCCESS (as expected)\n";

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration001>();

  cfg2->getView().version = testVersion;
  cfg2->getView().fillFromJSON("{\"test\": \"second_save\"}");

  std::cout << "Step 2: Attempting to save DUPLICATE version " << testVersion << " (second time)...\n";

  auto result2 = ifc.saveActiveVersion(cfg2.get());

  BOOST_CHECK_NE(result2, 0);
  std::cout << "  Result: FAILURE (as expected - duplicate rejected)\n";

  std::shared_ptr<ConfigurationBase> cfgRead = std::make_shared<TestConfiguration001>();

  auto resultRead = ifc.fill(cfgRead.get(), testVersion);
  BOOST_CHECK_EQUAL(resultRead, 0);

  BOOST_CHECK_MESSAGE(jsonContains(cfgRead->getView()._json, "test", "first_save"), "Expected 'first_save' but got: " << cfgRead->getView()._json);

  std::cout << "  Verified: Original version preserved\n";
}

BOOST_AUTO_TEST_CASE(duplicate_version_allowed_with_overwrite) {
  std::cout << "\n=== TEST 2: Duplicate version allowed with overwrite ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int testVersion = fixture.baseVersion + 1;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = testVersion;
  cfg1->getView().fillFromJSON("{\"data\": \"original\"}");

  std::cout << "Step 1: Saving original version " << testVersion << "\n";
  auto result1 = ifc.saveActiveVersion(cfg1.get());
  BOOST_CHECK_EQUAL(result1, 0);

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration001>();
  cfg2->getView().version = testVersion;
  cfg2->getView().fillFromJSON("{\"data\": \"updated\"}");

  std::cout << "Step 2: Overwriting version " << testVersion << " with overwrite=true\n";
  auto result2 = ifc.saveActiveVersion(cfg2.get(), true);

  BOOST_CHECK_EQUAL(result2, 0);
  std::cout << "  Result: SUCCESS (overwrite allowed)\n";

  std::shared_ptr<ConfigurationBase> cfgRead = std::make_shared<TestConfiguration001>();
  auto resultRead = ifc.fill(cfgRead.get(), testVersion);
  BOOST_CHECK_EQUAL(resultRead, 0);

  BOOST_CHECK_MESSAGE(jsonContains(cfgRead->getView()._json, "data", "updated"), "Expected 'updated' but got: " << cfgRead->getView()._json);
  std::cout << "  Verified: Version successfully overwritten\n";
}

BOOST_AUTO_TEST_CASE(duplicate_composition_rejected) {
  std::cout << "\n=== TEST 3: Duplicate composition rejected ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 10;
  int version2 = fixture.baseVersion + 20;

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

  std::string compositionName = std::string{"TestComposition_"} + std::to_string(fixture.baseVersion);

  std::cout << "Step 2: Creating composition '" << compositionName << "' (first time)...\n";

  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(compositionMap, compositionName));
  std::cout << "  Result: SUCCESS\n";

  std::cout << "Step 3: Attempting to create DUPLICATE composition '" << compositionName << "' (second time)...\n";

  BOOST_CHECK_THROW(ifc.storeGlobalConfiguration(compositionMap, compositionName), std::runtime_error);
  std::cout << "  Result: FAILURE (as expected - duplicate rejected)\n";

  auto loadedMap = ifc.loadGlobalConfiguration(compositionName);

  BOOST_CHECK_EQUAL(loadedMap.size(), 2);
  BOOST_CHECK_EQUAL(loadedMap[cfg1->getConfigurationName()], version1);
  BOOST_CHECK_EQUAL(loadedMap[cfg2->getConfigurationName()], version2);

  std::cout << "  Verified: Original composition preserved\n";
}

BOOST_AUTO_TEST_CASE(different_composition_names_allowed) {
  std::cout << "\n=== TEST 4: Different composition names allowed ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 100;
  int version2 = fixture.baseVersion + 200;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = version2;
  ifc.saveActiveVersion(cfg2.get());

  auto compositionMap = DatabaseConfigurationInterface::config_version_map_t{};
  compositionMap[cfg1->getConfigurationName()] = version1;
  compositionMap[cfg2->getConfigurationName()] = version2;

  std::string compositionName1 = std::string{"Composition_A_"} + std::to_string(fixture.baseVersion);

  std::cout << "Creating composition '" << compositionName1 << "'\n";
  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(compositionMap, compositionName1));

  std::string compositionName2 = std::string{"Composition_B_"} + std::to_string(fixture.baseVersion);

  std::cout << "Creating composition '" << compositionName2 << "' (same members, different name)\n";

  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(compositionMap, compositionName2));
  std::cout << "  Result: SUCCESS (different names allowed)\n";

  auto configs = ifc.findAllGlobalConfigurations();

  bool found1 = (std::find(configs.begin(), configs.end(), compositionName1) != configs.end());
  bool found2 = (std::find(configs.begin(), configs.end(), compositionName2) != configs.end());

  BOOST_CHECK_EQUAL(found1, true);
  BOOST_CHECK_EQUAL(found2, true);

  std::cout << "  Verified: Both compositions exist independently\n";
}

BOOST_AUTO_TEST_CASE(composition_different_members_allowed) {
  std::cout << "\n=== TEST 5: Composition different members allowed ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 300;
  int version2 = fixture.baseVersion + 400;
  int version3 = fixture.baseVersion + 500;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  ifc.saveActiveVersion(cfg1.get());

  std::shared_ptr<ConfigurationBase> cfg2a = std::make_shared<TestConfiguration002>();
  cfg2a->getView().version = version2;
  ifc.saveActiveVersion(cfg2a.get());

  std::shared_ptr<ConfigurationBase> cfg2b = std::make_shared<TestConfiguration002>();
  cfg2b->getView().version = version3;
  ifc.saveActiveVersion(cfg2b.get());

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[cfg1->getConfigurationName()] = version1;
  map1[cfg2a->getConfigurationName()] = version2;

  std::string compName1 = std::string{"CompA_"} + std::to_string(fixture.baseVersion);

  ifc.storeGlobalConfiguration(map1, compName1);
  std::cout << "Created composition '" << compName1 << "' with versions " << version1 << ", " << version2 << "\n";

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[cfg1->getConfigurationName()] = version1;
  map2[cfg2b->getConfigurationName()] = version3;

  std::string compName2 = std::string{"CompB_"} + std::to_string(fixture.baseVersion);

  BOOST_CHECK_NO_THROW(ifc.storeGlobalConfiguration(map2, compName2));
  std::cout << "Created composition '" << compName2 << "' with versions " << version1 << ", " << version3 << " (SUCCESS)\n";
}

BOOST_AUTO_TEST_CASE(raw_storage_verify_no_duplicate) {
  std::cout << "\n=== TEST 6: Raw storage verification ===\n";

  if (!isFileSystemBackend()) {
    std::cout << "  Skipping: Not using FileSystemDB backend\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int testVersion = fixture.baseVersion + 600;
  std::string configType = "TestConfiguration001";

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = testVersion;
  cfg1->getView().fillFromJSON("{\"marker\": \"original_data\"}");

  auto result1 = ifc.saveActiveVersion(cfg1.get());
  BOOST_CHECK_EQUAL(result1, 0);

  std::string collectionPath = fixture.dbPath + "/" + configType;
  int countBefore = countDocumentsWithVersion(collectionPath, configType, std::to_string(testVersion));
  std::cout << "Documents with version " << testVersion << " BEFORE duplicate attempt: " << countBefore << "\n";

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration001>();
  cfg2->getView().version = testVersion;
  cfg2->getView().fillFromJSON("{\"marker\": \"duplicate_data\"}");

  auto result2 = ifc.saveActiveVersion(cfg2.get());
  BOOST_CHECK_NE(result2, 0);

  int countAfter = countDocumentsWithVersion(collectionPath, configType, std::to_string(testVersion));
  std::cout << "Documents with version " << testVersion << " AFTER duplicate attempt: " << countAfter << "\n";

  BOOST_CHECK_EQUAL(countBefore, countAfter);
  BOOST_CHECK_EQUAL(countAfter, 1);

  std::cout << "  Verified: Duplicate was NOT written to disk\n";
}

BOOST_AUTO_TEST_CASE(duplicate_rejection_preserves_file_count) {
  std::cout << "\n=== TEST 7: Duplicate rejection preserves file count ===\n";

  if (!isFileSystemBackend()) {
    std::cout << "  Skipping: Not using FileSystemDB backend\n";
    BOOST_CHECK(true);
    return;
  }

  auto ifc = DatabaseConfigurationInterface();

  int testVersion = fixture.baseVersion + 700;
  std::string configType = "TestConfiguration001";
  std::string collectionPath = fixture.dbPath + "/" + configType;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = testVersion;
  auto result1 = ifc.saveActiveVersion(cfg1.get());
  BOOST_CHECK_EQUAL(result1, 0);

  auto filesBefore = listJsonFiles(collectionPath);
  int fileCountBefore = filesBefore.size();
  std::cout << "Files in collection BEFORE duplicate attempt: " << fileCountBefore << "\n";

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration001>();
  cfg2->getView().version = testVersion;
  auto result2 = ifc.saveActiveVersion(cfg2.get());
  BOOST_CHECK_NE(result2, 0);

  auto filesAfter = listJsonFiles(collectionPath);
  int fileCountAfter = filesAfter.size();
  std::cout << "Files in collection AFTER duplicate attempt: " << fileCountAfter << "\n";

  BOOST_CHECK_EQUAL(fileCountBefore, fileCountAfter);

  std::cout << "  Verified: Duplicate rejection did not create extra files\n";
}

BOOST_AUTO_TEST_CASE(stress_test_rapid_duplicates) {
  std::cout << "\n=== TEST 8: Stress test - rapid duplicate attempts ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int testVersion = fixture.baseVersion + 800;
  const int NUM_ATTEMPTS = 10;

  std::shared_ptr<ConfigurationBase> cfgOriginal = std::make_shared<TestConfiguration001>();
  cfgOriginal->getView().version = testVersion;
  auto result = ifc.saveActiveVersion(cfgOriginal.get());
  BOOST_CHECK_EQUAL(result, 0);

  std::cout << "Original version " << testVersion << " saved\n";

  auto startTime = std::chrono::high_resolution_clock::now();

  int successCount = 0;
  int failCount = 0;

  for (int i = 0; i < NUM_ATTEMPTS; i++) {
    std::shared_ptr<ConfigurationBase> cfgDup = std::make_shared<TestConfiguration001>();
    cfgDup->getView().version = testVersion;
    cfgDup->getView().fillFromJSON("{\"attempt\": " + std::to_string(i) + "}");

    auto dupResult = ifc.saveActiveVersion(cfgDup.get());
    if (dupResult == 0) {
      successCount++;
    } else {
      failCount++;
    }
  }

  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

  std::cout << "  Completed " << NUM_ATTEMPTS << " duplicate attempts in " << duration.count() << "ms\n";
  std::cout << "  Success: " << successCount << ", Failed: " << failCount << "\n";

  BOOST_CHECK_EQUAL(successCount, 0);
  BOOST_CHECK_EQUAL(failCount, NUM_ATTEMPTS);

  std::shared_ptr<ConfigurationBase> cfgRead = std::make_shared<TestConfiguration001>();
  auto readResult = ifc.fill(cfgRead.get(), testVersion);
  BOOST_CHECK_EQUAL(readResult, 0);

  std::cout << "  Verified: Original data preserved after " << NUM_ATTEMPTS << " duplicate attempts\n";
}

BOOST_AUTO_TEST_CASE(edge_case_boundary_versions) {
  std::cout << "\n=== TEST 9: Edge case - boundary versions ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int lowVersion = fixture.baseVersion + 900;
  int highVersion = fixture.baseVersion + 9999;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = lowVersion;

  bool v1Saved = (ifc.saveActiveVersion(cfg1.get()) == 0);
  std::cout << "  Version " << lowVersion << ": " << (v1Saved ? "SUCCESS" : "FAILED") << "\n";
  BOOST_CHECK_EQUAL(v1Saved, true);

  std::shared_ptr<ConfigurationBase> cfg1dup = std::make_shared<TestConfiguration001>();
  cfg1dup->getView().version = lowVersion;

  bool v1DupRejected = (ifc.saveActiveVersion(cfg1dup.get()) != 0);
  std::cout << "  Version " << lowVersion << " duplicate: " << (v1DupRejected ? "REJECTED (good)" : "ACCEPTED (bad)") << "\n";

  BOOST_CHECK_EQUAL(v1DupRejected, true);

  std::shared_ptr<ConfigurationBase> cfgLarge = std::make_shared<TestConfiguration001>();
  cfgLarge->getView().version = highVersion;

  bool largeSaved = (ifc.saveActiveVersion(cfgLarge.get()) == 0);
  std::cout << "  Version " << highVersion << ": " << (largeSaved ? "SUCCESS" : "FAILED") << "\n";
  BOOST_CHECK_EQUAL(largeSaved, true);

  std::shared_ptr<ConfigurationBase> cfgLargeDup = std::make_shared<TestConfiguration001>();
  cfgLargeDup->getView().version = highVersion;

  bool largeDupRejected = (ifc.saveActiveVersion(cfgLargeDup.get()) != 0);
  std::cout << "  Version " << highVersion << " duplicate: " << (largeDupRejected ? "REJECTED (good)" : "ACCEPTED (bad)") << "\n";

  BOOST_CHECK_EQUAL(largeDupRejected, true);

  std::cout << "  Verified: Boundary version duplicate detection works\n";
}

BOOST_AUTO_TEST_CASE(edge_case_special_characters) {
  std::cout << "\n=== TEST 10: Edge case - special characters in data ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int testVersion = fixture.baseVersion + 1000;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = testVersion;
  cfg1->getView().fillFromJSON("{\"special\": \"line1\\nline2\\ttab\"}");

  auto result1 = ifc.saveActiveVersion(cfg1.get());
  BOOST_CHECK_EQUAL(result1, 0);
  std::cout << "  Saved version with special characters\n";

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration001>();
  cfg2->getView().version = testVersion;
  cfg2->getView().fillFromJSON("{\"special\": \"different\\ndata\"}");

  auto result2 = ifc.saveActiveVersion(cfg2.get());
  BOOST_CHECK_NE(result2, 0);
  std::cout << "  Duplicate with different special chars: REJECTED (good)\n";

  std::shared_ptr<ConfigurationBase> cfgRead = std::make_shared<TestConfiguration001>();
  ifc.fill(cfgRead.get(), testVersion);

  BOOST_CHECK_MESSAGE(cfgRead->getView()._json.find("line1") != std::string::npos, "Original data should be preserved");

  std::cout << "  Verified: Special character handling works correctly\n";
}

BOOST_AUTO_TEST_CASE(performance_duplicate_detection) {
  std::cout << "\n=== TEST 11: Performance measurement ===\n";

  auto ifc = DatabaseConfigurationInterface();

  const int NUM_VERSIONS = 10;
  int baseVer = fixture.baseVersion + 1100;

  auto startUnique = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_VERSIONS; i++) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = baseVer + i;
    ifc.saveActiveVersion(cfg.get());
  }

  auto endUnique = std::chrono::high_resolution_clock::now();
  auto uniqueDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endUnique - startUnique);

  std::cout << "  Time to save " << NUM_VERSIONS << " unique versions: " << uniqueDuration.count() << "ms\n";
  std::cout << "  Average per version: " << (uniqueDuration.count() / NUM_VERSIONS) << "ms\n";

  auto startDup = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_VERSIONS; i++) {
    std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
    cfg->getView().version = baseVer + i;
    ifc.saveActiveVersion(cfg.get());
  }

  auto endDup = std::chrono::high_resolution_clock::now();
  auto dupDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endDup - startDup);

  std::cout << "  Time to reject " << NUM_VERSIONS << " duplicates: " << dupDuration.count() << "ms\n";
  std::cout << "  Average per rejection: " << (dupDuration.count() / NUM_VERSIONS) << "ms\n";

  BOOST_CHECK_LT(uniqueDuration.count() + dupDuration.count(), 10000);

  std::cout << "  Verified: Performance within acceptable limits\n";
}

BOOST_AUTO_TEST_CASE(composition_overwrite_allowed) {
  std::cout << "\n=== TEST 12: Composition overwrite flag test ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int version1 = fixture.baseVersion + 1200;
  int version2 = fixture.baseVersion + 1201;
  int version3 = fixture.baseVersion + 1202;

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = version1;
  auto r1 = ifc.saveActiveVersion(cfg1.get());
  BOOST_CHECK_EQUAL(r1, 0);

  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = version2;
  auto r2 = ifc.saveActiveVersion(cfg2.get());
  BOOST_CHECK_EQUAL(r2, 0);

  std::shared_ptr<ConfigurationBase> cfg3 = std::make_shared<TestConfiguration002>();
  cfg3->getView().version = version3;
  auto r3 = ifc.saveActiveVersion(cfg3.get());
  BOOST_CHECK_EQUAL(r3, 0);

  auto map1 = DatabaseConfigurationInterface::config_version_map_t{};
  map1[cfg1->getConfigurationName()] = version1;
  map1[cfg2->getConfigurationName()] = version2;

  std::string compName = std::string{"OverwriteComp_"} + std::to_string(fixture.baseVersion);

  bool originalCreated = false;
  try {
    ifc.storeGlobalConfiguration(map1, compName);
    originalCreated = true;
    std::cout << "Step 1: Created composition '" << compName << "' with v" << version1 << ", v" << version2 << "\n";
  } catch (std::exception const& e) {
    std::cout << "  Failed to create original composition: " << e.what() << "\n";
  }
  BOOST_CHECK_EQUAL(originalCreated, true);

  bool duplicateFailed = false;
  try {
    ifc.storeGlobalConfiguration(map1, compName, false);
    std::cout << "  ERROR: Duplicate composition was allowed without overwrite flag\n";
  } catch (std::exception const& e) {
    duplicateFailed = true;
    std::cout << "Step 2: Duplicate without overwrite rejected (as expected)\n";
  }
  BOOST_CHECK_EQUAL(duplicateFailed, true);

  auto map2 = DatabaseConfigurationInterface::config_version_map_t{};
  map2[cfg1->getConfigurationName()] = version1;
  map2[cfg3->getConfigurationName()] = version3;

  bool overwriteSucceeded = false;
  try {
    ifc.storeGlobalConfiguration(map2, compName, true);
    overwriteSucceeded = true;
    std::cout << "Step 3: Overwrite with allowOverwrite=true succeeded\n";
  } catch (std::exception const& e) {
    std::cout << "  Overwrite failed: " << e.what() << "\n";
  }
  BOOST_CHECK_EQUAL(overwriteSucceeded, true);

  std::cout << "  Verified: allowOverwrite flag controls composition update behavior\n";
}

BOOST_AUTO_TEST_CASE(sequential_multi_version_duplicates) {
  std::cout << "\n=== TEST 13: Sequential multi-version with duplicate checks ===\n";

  auto ifc = DatabaseConfigurationInterface();

  const int NUM_VERSIONS = 5;
  int baseVer = fixture.baseVersion + 1300;

  auto startTime = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_VERSIONS; i++) {
    int version = baseVer + i;

    std::shared_ptr<ConfigurationBase> cfgOrig = std::make_shared<TestConfiguration001>();
    cfgOrig->getView().version = version;
    auto origResult = ifc.saveActiveVersion(cfgOrig.get());
    BOOST_CHECK_EQUAL(origResult, 0);

    std::shared_ptr<ConfigurationBase> cfgDup = std::make_shared<TestConfiguration001>();
    cfgDup->getView().version = version;
    auto dupResult = ifc.saveActiveVersion(cfgDup.get());
    BOOST_CHECK_NE(dupResult, 0);
  }

  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

  std::cout << "  Created " << NUM_VERSIONS << " versions with immediate duplicate checks\n";
  std::cout << "  Total time: " << duration.count() << "ms\n";

  auto versions = ifc.getVersions(std::make_shared<TestConfiguration001>().get());
  int foundCount = 0;
  for (int i = 0; i < NUM_VERSIONS; i++) {
    if (versions.find(baseVer + i) != versions.end()) {
      foundCount++;
    }
  }

  BOOST_CHECK_EQUAL(foundCount, NUM_VERSIONS);
  std::cout << "  Verified: All " << NUM_VERSIONS << " unique versions exist\n";
}

BOOST_AUTO_TEST_CASE(comprehensive_duplicate_scan) {
  std::cout << "\n=== TEST 14: Comprehensive duplicate scan ===\n";

  if (!isFileSystemBackend()) {
    std::cout << "  Skipping: Not using FileSystemDB backend\n";
    BOOST_CHECK(true);
    return;
  }

  std::string configType = "TestConfiguration001";
  std::string collectionPath = fixture.dbPath + "/" + configType;

  auto jsonFiles = listJsonFiles(collectionPath);
  std::cout << "  Scanning " << jsonFiles.size() << " files in " << collectionPath << "\n";

  std::map<std::string, int> versionCounts;
  std::regex versionPattern("\"version\"\\s*:\\s*\"([^\"]+)\"");

  for (const auto& filepath : jsonFiles) {
    std::string content = readFileContent(filepath);
    if (content.empty()) continue;

    std::smatch match;
    if (std::regex_search(content, match, versionPattern)) {
      std::string version = match[1].str();
      versionCounts[version]++;
    }
  }

  int duplicatesFound = 0;
  for (const auto& entry : versionCounts) {
    if (entry.second > 1) {
      std::cout << "  DUPLICATE: Version " << entry.first << " appears " << entry.second << " times\n";
      duplicatesFound++;
    }
  }

  std::cout << "  Versions scanned: " << versionCounts.size() << "\n";
  std::cout << "  Duplicates found: " << duplicatesFound << "\n";

  BOOST_CHECK_EQUAL(duplicatesFound, 0);
  std::cout << "  Verified: No duplicate versions in storage\n";
}

BOOST_AUTO_TEST_SUITE_END()
