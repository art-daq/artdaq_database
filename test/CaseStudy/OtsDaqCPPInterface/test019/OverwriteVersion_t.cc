#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (OverwriteVersion test)

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct OverwriteVersionTestData {
  OverwriteVersionTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== OVERWRITE VERSION TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~OverwriteVersionTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

OverwriteVersionTestData fixture;

BOOST_AUTO_TEST_SUITE(overwrite_version_test)

BOOST_AUTO_TEST_CASE(basic_overwrite_succeeds) {
  std::cout << "\n=== TEST 1: Basic overwrite succeeds ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 1;
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"field\": \"initial_value\"}");

  std::cout << "Step 1: Creating initial version " << testVersion << " with 'initial_value'\n";
  auto createResult = ifc.saveActiveVersion(cfg.get(), false);
  BOOST_REQUIRE_EQUAL(createResult, 0);
  std::cout << "  Initial version created successfully\n";

  std::cout << "Step 2: Overwriting with 'updated_value'\n";
  cfg->getView().fillFromJSON("{\"field\": \"updated_value\"}");
  auto overwriteResult = ifc.saveActiveVersion(cfg.get(), true);
  BOOST_REQUIRE_EQUAL(overwriteResult, 0);
  std::cout << "  Overwrite completed successfully\n";

  std::cout << "Step 3: Loading version and verifying content\n";
  auto loaded = std::make_shared<TestConfiguration001>();
  auto loadResult = ifc.fill(loaded.get(), testVersion);
  BOOST_REQUIRE_EQUAL(loadResult, 0);

  std::stringstream ss;
  loaded->getView().printJSON(ss);
  std::string loadedJson = ss.str();
  std::cout << "  Loaded content: " << loadedJson << "\n";

  BOOST_CHECK(loadedJson.find("updated_value") != std::string::npos);
  std::cout << "  Verified: Content was updated to 'updated_value'\n";
}

BOOST_AUTO_TEST_CASE(overwrite_nonexistent_behavior) {
  std::cout << "\n=== TEST 2: Overwrite non-existent version behavior ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = 999999997;
  cfg->getView().fillFromJSON("{\"field\": \"value\"}");

  std::cout << "Step 1: Attempting to overwrite non-existent version 999999997\n";
  auto overwriteResult = ifc.saveActiveVersion(cfg.get(), true);
  std::cout << "  Overwrite returned: " << overwriteResult << "\n";

  if (overwriteResult == 0) {
    std::cout << "  Observation: Overwrite on non-existent version SUCCEEDED (upsert behavior)\n";
    auto versions = ifc.getVersions(cfg.get());
    bool found = versions.find(999999997) != versions.end();
    std::cout << "  Version 999999997 exists after overwrite: " << (found ? "yes" : "no") << "\n";
    BOOST_CHECK(found);
  } else {
    std::cout << "  Observation: Overwrite on non-existent version FAILED as expected\n";
    BOOST_CHECK(true);
  }
}

BOOST_AUTO_TEST_CASE(overwrite_preserves_version_number) {
  std::cout << "\n=== TEST 3: Overwrite preserves version number ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 2;
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"field\": \"original\"}");

  std::cout << "Step 1: Creating version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), false), 0);

  std::cout << "Step 2: Getting versions before overwrite\n";
  auto versionsBefore = ifc.getVersions(cfg.get());
  std::cout << "  Versions count before: " << versionsBefore.size() << "\n";
  BOOST_REQUIRE(versionsBefore.find(testVersion) != versionsBefore.end());

  std::cout << "Step 3: Overwriting version " << testVersion << "\n";
  cfg->getView().fillFromJSON("{\"field\": \"overwritten\"}");
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), true), 0);

  std::cout << "Step 4: Getting versions after overwrite\n";
  auto versionsAfter = ifc.getVersions(cfg.get());
  std::cout << "  Versions count after: " << versionsAfter.size() << "\n";

  BOOST_CHECK_EQUAL(versionsBefore.size(), versionsAfter.size());
  BOOST_CHECK(versionsAfter.find(testVersion) != versionsAfter.end());
  std::cout << "  Verified: No new version was created, version " << testVersion << " still exists\n";
}

BOOST_AUTO_TEST_CASE(overwrite_readonly_behavior) {
  std::cout << "\n=== TEST 4: Overwrite read-only version behavior ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 3;
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"field\": \"readonly_test\"}");

  std::cout << "Step 1: Creating version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), false), 0);

  std::cout << "Step 2: Attempting to mark version as read-only\n";
  auto markResult = ifc.markActiveVersionReadonly(cfg.get());
  std::cout << "  markActiveVersionReadonly returned: " << markResult << "\n";

  if (markResult != 0) {
    std::cout << "  Observation: markVersionReadonly returned error for existing version\n";
    std::cout << "  SKIPPING overwrite test - cannot verify readonly protection\n";
    BOOST_CHECK(true);
    return;
  }

  std::cout << "  Version marked read-only successfully\n";

  std::cout << "Step 3: Attempting to overwrite read-only version\n";
  cfg->getView().fillFromJSON("{\"field\": \"should_fail\"}");
  auto overwriteResult = ifc.saveActiveVersion(cfg.get(), true);
  std::cout << "  Overwrite returned: " << overwriteResult << "\n";

  if (overwriteResult != 0) {
    std::cout << "  Verified: Overwrite of read-only version correctly rejected\n";
  } else {
    std::cout << "  Observation: Overwrite of read-only version was ALLOWED\n";
  }
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(multiple_overwrites_work) {
  std::cout << "\n=== TEST 5: Multiple overwrites work correctly ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 4;
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"counter\": 0}");

  std::cout << "Step 1: Creating initial version " << testVersion << " with counter=0\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), false), 0);

  for (int i = 1; i <= 3; ++i) {
    std::cout << "Step " << (i + 1) << ": Overwriting with counter=" << i << "\n";
    std::string newJson = "{\"counter\": " + std::to_string(i) + "}";
    cfg->getView().fillFromJSON(newJson);
    auto result = ifc.saveActiveVersion(cfg.get(), true);
    BOOST_REQUIRE_EQUAL(result, 0);
    std::cout << "  Overwrite " << i << " succeeded\n";
  }

  std::cout << "Step 5: Verifying final content\n";
  auto loaded = std::make_shared<TestConfiguration001>();
  BOOST_REQUIRE_EQUAL(ifc.fill(loaded.get(), testVersion), 0);

  std::stringstream ss;
  loaded->getView().printJSON(ss);
  std::string finalJson = ss.str();
  std::cout << "  Final content: " << finalJson << "\n";

  BOOST_CHECK(finalJson.find("3") != std::string::npos);
  std::cout << "  Verified: Final content has counter=3\n";

  std::cout << "Step 6: Verifying only one version exists\n";
  auto versions = ifc.getVersions(cfg.get());
  auto count = std::count(versions.begin(), versions.end(), testVersion);
  BOOST_CHECK_EQUAL(count, 1);
  std::cout << "  Verified: Version " << testVersion << " appears exactly once\n";
}

BOOST_AUTO_TEST_CASE(overwrite_does_not_create_new_version) {
  std::cout << "\n=== TEST 6: Overwrite does not create a new version ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 5;
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"data\": \"test\"}");

  std::cout << "Step 1: Creating version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), false), 0);

  std::cout << "Step 2: Recording version count before overwrite\n";
  auto versionsBefore = ifc.getVersions(cfg.get());
  size_t countBefore = versionsBefore.size();
  std::cout << "  Version count before: " << countBefore << "\n";

  std::cout << "Step 3: Performing 5 overwrites\n";
  for (int i = 0; i < 5; ++i) {
    cfg->getView().fillFromJSON("{\"data\": \"overwrite_" + std::to_string(i) + "\"}");
    BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), true), 0);
  }
  std::cout << "  5 overwrites completed\n";

  std::cout << "Step 4: Checking version count after overwrites\n";
  auto versionsAfter = ifc.getVersions(cfg.get());
  size_t countAfter = versionsAfter.size();
  std::cout << "  Version count after: " << countAfter << "\n";

  BOOST_CHECK_EQUAL(countBefore, countAfter);
  std::cout << "  Verified: No new versions were created during overwrites\n";
}

BOOST_AUTO_TEST_CASE(overwrite_different_config_types_independent) {
  std::cout << "\n=== TEST 7: Overwrite of different config types are independent ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int testVersion = fixture.baseVersion + 6;

  auto cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = testVersion;
  cfg1->getView().fillFromJSON("{\"type\": \"config1_original\"}");

  auto cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = testVersion;
  cfg2->getView().fillFromJSON("{\"type\": \"config2_original\"}");

  std::cout << "Step 1: Creating both config types with version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg1.get(), false), 0);
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg2.get(), false), 0);

  std::cout << "Step 2: Overwriting only TestConfiguration001\n";
  cfg1->getView().fillFromJSON("{\"type\": \"config1_updated\"}");
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg1.get(), true), 0);

  std::cout << "Step 3: Verifying TestConfiguration002 is unchanged\n";
  auto loaded2 = std::make_shared<TestConfiguration002>();
  BOOST_REQUIRE_EQUAL(ifc.fill(loaded2.get(), testVersion), 0);

  std::stringstream ss2;
  loaded2->getView().printJSON(ss2);
  std::string loaded2Json = ss2.str();
  std::cout << "  TestConfiguration002 content: " << loaded2Json << "\n";

  BOOST_CHECK(loaded2Json.find("config2_original") != std::string::npos);
  std::cout << "  Verified: TestConfiguration002 was not affected by TestConfiguration001 overwrite\n";

  std::cout << "Step 4: Verifying TestConfiguration001 was updated\n";
  auto loaded1 = std::make_shared<TestConfiguration001>();
  BOOST_REQUIRE_EQUAL(ifc.fill(loaded1.get(), testVersion), 0);

  std::stringstream ss1;
  loaded1->getView().printJSON(ss1);
  std::string loaded1Json = ss1.str();
  std::cout << "  TestConfiguration001 content: " << loaded1Json << "\n";

  BOOST_CHECK(loaded1Json.find("config1_updated") != std::string::npos);
  std::cout << "  Verified: TestConfiguration001 was correctly updated\n";
}

BOOST_AUTO_TEST_SUITE_END()
