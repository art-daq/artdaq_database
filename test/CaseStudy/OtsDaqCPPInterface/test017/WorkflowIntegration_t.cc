#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (WorkflowIntegration test)

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct WorkflowIntegrationTestData {
  WorkflowIntegrationTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== WORKFLOW INTEGRATION TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~WorkflowIntegrationTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

WorkflowIntegrationTestData fixture;

BOOST_AUTO_TEST_SUITE(workflow_integration_test)

BOOST_AUTO_TEST_CASE(complete_crud_lifecycle) {
  std::cout << "\n=== TEST 1: Complete CRUD lifecycle ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int version1 = fixture.baseVersion;
  int version2 = fixture.baseVersion + 1;

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = version1;
  cfg->getView().fillFromJSON("{\"stage\": \"created\"}");
  auto createResult = ifc.saveActiveVersion(cfg.get());
  BOOST_REQUIRE_EQUAL(createResult, 0);
  std::cout << "Step 1 (CREATE): Saved version " << version1 << "\n";

  std::shared_ptr<ConfigurationBase> readCfg = std::make_shared<TestConfiguration001>();
  auto readResult = ifc.fill(readCfg.get(), version1);
  BOOST_REQUIRE_EQUAL(readResult, 0);
  bool hasCreated = (readCfg->getView()._json.find("created") != std::string::npos);
  BOOST_CHECK(hasCreated);
  std::cout << "Step 2 (READ): Loaded version " << version1 << ", has 'created': " << (hasCreated ? "yes" : "no") << "\n";

  cfg->getView().version = version2;
  cfg->getView().fillFromJSON("{\"stage\": \"updated\"}");
  auto updateResult = ifc.saveActiveVersion(cfg.get());
  BOOST_REQUIRE_EQUAL(updateResult, 0);
  std::cout << "Step 3 (UPDATE): Saved version " << version2 << "\n";

  auto versions = ifc.getVersions(cfg.get());
  bool hasV1 = (versions.find(version1) != versions.end());
  bool hasV2 = (versions.find(version2) != versions.end());
  BOOST_CHECK(hasV1);
  BOOST_CHECK(hasV2);
  std::cout << "Step 4 (VERIFY): Has v1: " << (hasV1 ? "yes" : "no") << ", Has v2: " << (hasV2 ? "yes" : "no") << "\n";

  std::cout << "  Verified: Complete CRUD lifecycle works\n";
}

BOOST_AUTO_TEST_CASE(version_promotion_workflow) {
  std::cout << "\n=== TEST 2: Version promotion workflow ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int testVersion = fixture.baseVersion + 10;
  std::string suffix = "_" + std::to_string(fixture.baseVersion);

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"environment\": \"development\"}");
  ifc.saveActiveVersion(cfg.get());
  std::cout << "Step 1: Created development version " << testVersion << "\n";

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[cfg->getConfigurationName()] = testVersion;
  ifc.storeGlobalConfiguration(map, "Development" + suffix);
  std::cout << "Step 2: Created Development composition\n";

  ifc.storeGlobalConfiguration(map, "Staging" + suffix);
  std::cout << "Step 3: Promoted to Staging composition\n";

  ifc.storeGlobalConfiguration(map, "Production" + suffix);
  std::cout << "Step 4: Promoted to Production composition\n";

  auto devLoaded = ifc.loadGlobalConfiguration("Development" + suffix);
  auto stagingLoaded = ifc.loadGlobalConfiguration("Staging" + suffix);
  auto prodLoaded = ifc.loadGlobalConfiguration("Production" + suffix);

  BOOST_CHECK_EQUAL(devLoaded[cfg->getConfigurationName()], testVersion);
  BOOST_CHECK_EQUAL(stagingLoaded[cfg->getConfigurationName()], testVersion);
  BOOST_CHECK_EQUAL(prodLoaded[cfg->getConfigurationName()], testVersion);

  std::cout << "Step 5: All compositions reference version " << testVersion << "\n";
  std::cout << "  Verified: Version promotion workflow works\n";
}

BOOST_AUTO_TEST_CASE(rollback_simulation) {
  std::cout << "\n=== TEST 3: Rollback simulation ===\n";

  auto ifc = DatabaseConfigurationInterface();
  int goodVersion = fixture.baseVersion + 20;
  int badVersion = fixture.baseVersion + 21;
  std::string compName = "RollbackTest_" + std::to_string(fixture.baseVersion);

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = goodVersion;
  cfg->getView().fillFromJSON("{\"status\": \"good_version\"}");
  ifc.saveActiveVersion(cfg.get());
  std::cout << "Step 1: Created good version " << goodVersion << "\n";

  cfg->getView().version = badVersion;
  cfg->getView().fillFromJSON("{\"status\": \"bad_version\"}");
  ifc.saveActiveVersion(cfg.get());
  std::cout << "Step 2: Created bad version " << badVersion << "\n";

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[cfg->getConfigurationName()] = badVersion;
  ifc.storeGlobalConfiguration(map, compName);
  std::cout << "Step 3: Created composition pointing to bad version\n";

  auto loaded1 = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded1[cfg->getConfigurationName()], badVersion);

  map[cfg->getConfigurationName()] = goodVersion;
  ifc.storeGlobalConfiguration(map, compName, true);
  std::cout << "Step 4: Rolled back composition to good version\n";

  auto loaded2 = ifc.loadGlobalConfiguration(compName);
  BOOST_CHECK_EQUAL(loaded2[cfg->getConfigurationName()], goodVersion);

  std::cout << "Step 5: Verified composition now points to version " << loaded2[cfg->getConfigurationName()] << "\n";
  std::cout << "  Verified: Rollback simulation works\n";
}

BOOST_AUTO_TEST_CASE(parallel_development_branches) {
  std::cout << "\n=== TEST 4: Parallel development branches ===\n";

  auto ifc = DatabaseConfigurationInterface();
  std::string suffix = "_" + std::to_string(fixture.baseVersion);

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();

  int mainVersion = fixture.baseVersion + 30;
  cfg->getView().version = mainVersion;
  cfg->getView().fillFromJSON("{\"branch\": \"main\"}");
  ifc.saveActiveVersion(cfg.get());

  for (int i = 0; i < 3; i++) {
    cfg->getView().version = fixture.baseVersion + 100 + i;
    cfg->getView().fillFromJSON("{\"branch\": \"featureA\", \"iteration\": " + std::to_string(i) + "}");
    ifc.saveActiveVersion(cfg.get());
  }

  for (int i = 0; i < 3; i++) {
    cfg->getView().version = fixture.baseVersion + 200 + i;
    cfg->getView().fillFromJSON("{\"branch\": \"featureB\", \"iteration\": " + std::to_string(i) + "}");
    ifc.saveActiveVersion(cfg.get());
  }
  std::cout << "Step 1: Created versions for main, featureA, and featureB\n";

  auto mainMap = DatabaseConfigurationInterface::config_version_map_t{{cfg->getConfigurationName(), mainVersion}};
  auto featureAMap = DatabaseConfigurationInterface::config_version_map_t{{cfg->getConfigurationName(), fixture.baseVersion + 102}};
  auto featureBMap = DatabaseConfigurationInterface::config_version_map_t{{cfg->getConfigurationName(), fixture.baseVersion + 202}};

  ifc.storeGlobalConfiguration(mainMap, "Main" + suffix);
  ifc.storeGlobalConfiguration(featureAMap, "FeatureA" + suffix);
  ifc.storeGlobalConfiguration(featureBMap, "FeatureB" + suffix);
  std::cout << "Step 2: Created compositions for each branch\n";

  auto mainLoaded = ifc.loadGlobalConfiguration("Main" + suffix);
  auto featureALoaded = ifc.loadGlobalConfiguration("FeatureA" + suffix);
  auto featureBLoaded = ifc.loadGlobalConfiguration("FeatureB" + suffix);

  BOOST_CHECK_EQUAL(mainLoaded[cfg->getConfigurationName()], mainVersion);
  BOOST_CHECK_EQUAL(featureALoaded[cfg->getConfigurationName()], fixture.baseVersion + 102);
  BOOST_CHECK_EQUAL(featureBLoaded[cfg->getConfigurationName()], fixture.baseVersion + 202);

  std::cout << "Step 3: Main=" << mainLoaded[cfg->getConfigurationName()] << ", FeatureA=" << featureALoaded[cfg->getConfigurationName()]
            << ", FeatureB=" << featureBLoaded[cfg->getConfigurationName()] << "\n";
  std::cout << "  Verified: Parallel development branches work independently\n";
}

BOOST_AUTO_TEST_CASE(composition_evolution_history) {
  std::cout << "\n=== TEST 5: Composition evolution history ===\n";

  auto ifc = DatabaseConfigurationInterface();
  std::string compName = "Evolving_" + std::to_string(fixture.baseVersion);

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();

  cfg1->getView().version = fixture.baseVersion + 40;
  cfg2->getView().version = fixture.baseVersion + 40;
  ifc.saveActiveVersion(cfg1.get());
  ifc.saveActiveVersion(cfg2.get());

  cfg1->getView().version = fixture.baseVersion + 41;
  ifc.saveActiveVersion(cfg1.get());
  std::cout << "Step 1: Created configuration versions\n";

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[cfg1->getConfigurationName()] = fixture.baseVersion + 40;
  ifc.storeGlobalConfiguration(map, compName);

  auto loaded1 = ifc.loadGlobalConfiguration(compName);
  std::cout << "Step 2: Initial composition has " << loaded1.size() << " member(s)\n";
  BOOST_CHECK_EQUAL(loaded1.size(), 1);

  map[cfg2->getConfigurationName()] = fixture.baseVersion + 40;
  ifc.storeGlobalConfiguration(map, compName, true);

  auto loaded2 = ifc.loadGlobalConfiguration(compName);
  std::cout << "Step 3: After adding member, has " << loaded2.size() << " member(s)\n";
  BOOST_CHECK_EQUAL(loaded2.size(), 2);

  map[cfg1->getConfigurationName()] = fixture.baseVersion + 41;
  ifc.storeGlobalConfiguration(map, compName, true);

  auto loaded3 = ifc.loadGlobalConfiguration(compName);
  std::cout << "Step 4: After version update, type1 version is " << loaded3[cfg1->getConfigurationName()] << "\n";
  BOOST_CHECK_EQUAL(loaded3[cfg1->getConfigurationName()], fixture.baseVersion + 41);
  BOOST_CHECK_EQUAL(loaded3.size(), 2);

  map.erase(cfg2->getConfigurationName());
  ifc.storeGlobalConfiguration(map, compName, true);

  auto loaded4 = ifc.loadGlobalConfiguration(compName);
  std::cout << "Step 5: After removing member, has " << loaded4.size() << " member(s)\n";
  BOOST_CHECK_EQUAL(loaded4.size(), 1);
  BOOST_CHECK_EQUAL(loaded4[cfg1->getConfigurationName()], fixture.baseVersion + 41);

  std::cout << "  Verified: Composition evolution works correctly\n";
}

BOOST_AUTO_TEST_SUITE_END()
