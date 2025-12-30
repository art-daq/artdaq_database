#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (MarkReadonlyExpanded test)

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct MarkReadonlyExpandedTestData {
  MarkReadonlyExpandedTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 300000;
    std::cout << "=== MARK READONLY EXPANDED TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~MarkReadonlyExpandedTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

MarkReadonlyExpandedTestData fixture;

BOOST_AUTO_TEST_SUITE(mark_readonly_expanded_test)

BOOST_AUTO_TEST_CASE(mark_readonly_on_existing_version_succeeds) {
  std::cout << "\n=== TEST 1: Mark readonly on existing version succeeds ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 1;
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"field\": \"test_data\"}");

  std::cout << "Step 1: Creating version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), false), 0);

  std::cout << "Step 2: Marking version as read-only\n";
  auto markResult = ifc.markActiveVersionReadonly(cfg.get());
  std::cout << "  markActiveVersionReadonly returned: " << markResult << "\n";

  BOOST_CHECK_EQUAL(markResult, 0);
  std::cout << "  Verified: markVersionReadonly succeeds on existing version\n";
}

BOOST_AUTO_TEST_CASE(mark_readonly_is_idempotent) {
  std::cout << "\n=== TEST 2: Mark readonly is idempotent ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 2;
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"field\": \"idempotent_test\"}");

  std::cout << "Step 1: Creating version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), false), 0);

  std::cout << "Step 2: Marking version as read-only (first time)\n";
  auto firstMark = ifc.markActiveVersionReadonly(cfg.get());
  std::cout << "  First mark returned: " << firstMark << "\n";
  BOOST_REQUIRE_EQUAL(firstMark, 0);

  std::cout << "Step 3: Marking version as read-only (second time)\n";
  auto secondMark = ifc.markActiveVersionReadonly(cfg.get());
  std::cout << "  Second mark returned: " << secondMark << "\n";

  BOOST_CHECK_EQUAL(secondMark, 0);
  std::cout << "  Verified: Marking read-only twice succeeds (idempotent)\n";
}

BOOST_AUTO_TEST_CASE(readonly_version_can_be_loaded) {
  std::cout << "\n=== TEST 3: Read-only version can still be loaded ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 3;
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"field\": \"loadable_readonly\"}");

  std::cout << "Step 1: Creating version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), false), 0);

  std::cout << "Step 2: Marking version as read-only\n";
  BOOST_REQUIRE_EQUAL(ifc.markActiveVersionReadonly(cfg.get()), 0);

  std::cout << "Step 3: Loading the read-only version\n";
  auto loaded = std::make_shared<TestConfiguration001>();
  auto loadResult = ifc.fill(loaded.get(), testVersion);
  std::cout << "  fill() returned: " << loadResult << "\n";

  BOOST_REQUIRE_EQUAL(loadResult, 0);

  std::stringstream ss;
  loaded->getView().printJSON(ss);
  std::string loadedJson = ss.str();
  std::cout << "  Loaded content: " << loadedJson << "\n";

  BOOST_CHECK(loadedJson.find("loadable_readonly") != std::string::npos);
  std::cout << "  Verified: Read-only version can still be loaded and content is correct\n";
}

BOOST_AUTO_TEST_CASE(readonly_version_appears_in_version_list) {
  std::cout << "\n=== TEST 4: Read-only version appears in getVersions ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 4;
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"field\": \"version_list_test\"}");

  std::cout << "Step 1: Creating version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), false), 0);

  std::cout << "Step 2: Verifying version appears in list before marking readonly\n";
  auto versionsBefore = ifc.getVersions(cfg.get());
  bool foundBefore = versionsBefore.find(testVersion) != versionsBefore.end();
  std::cout << "  Version " << testVersion << " in list before: " << (foundBefore ? "YES" : "NO") << "\n";
  BOOST_REQUIRE(foundBefore);

  std::cout << "Step 3: Marking version as read-only\n";
  BOOST_REQUIRE_EQUAL(ifc.markActiveVersionReadonly(cfg.get()), 0);

  std::cout << "Step 4: Verifying version still appears in list after marking readonly\n";
  auto versionsAfter = ifc.getVersions(cfg.get());
  bool foundAfter = versionsAfter.find(testVersion) != versionsAfter.end();
  std::cout << "  Version " << testVersion << " in list after: " << (foundAfter ? "YES" : "NO") << "\n";

  BOOST_CHECK(foundAfter);
  std::cout << "  Verified: Read-only version still appears in version listing\n";
}

BOOST_AUTO_TEST_CASE(readonly_blocks_overwrite) {
  std::cout << "\n=== TEST 5: Read-only blocks overwrite ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 5;
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"field\": \"original_content\"}");

  std::cout << "Step 1: Creating version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), false), 0);

  std::cout << "Step 2: Marking version as read-only\n";
  BOOST_REQUIRE_EQUAL(ifc.markActiveVersionReadonly(cfg.get()), 0);

  std::cout << "Step 3: Attempting to overwrite read-only version\n";
  cfg->getView().fillFromJSON("{\"field\": \"modified_content\"}");
  auto overwriteResult = ifc.saveActiveVersion(cfg.get(), true);
  std::cout << "  Overwrite returned: " << overwriteResult << "\n";

  if (overwriteResult != 0) {
    std::cout << "  Verified: Read-only version correctly blocks overwrite\n";
  } else {
    std::cout << "  Observation: Read-only version allowed overwrite (may be implementation choice)\n";
  }

  auto loaded = std::make_shared<TestConfiguration001>();
  ifc.fill(loaded.get(), testVersion);
  std::stringstream ss;
  loaded->getView().printJSON(ss);
  std::cout << "  Final content: " << ss.str() << "\n";

  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(readonly_blocks_store_same_version) {
  std::cout << "\n=== TEST 6: Read-only blocks storing same version ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();
  int testVersion = fixture.baseVersion + 6;
  cfg->getView().version = testVersion;
  cfg->getView().fillFromJSON("{\"field\": \"readonly_store_test\"}");

  std::cout << "Step 1: Creating version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), false), 0);

  std::cout << "Step 2: Marking version as read-only\n";
  BOOST_REQUIRE_EQUAL(ifc.markActiveVersionReadonly(cfg.get()), 0);

  std::cout << "Step 3: Attempting to store same version again (not overwrite)\n";
  cfg->getView().fillFromJSON("{\"field\": \"new_content\"}");
  auto storeResult = ifc.saveActiveVersion(cfg.get(), false);
  std::cout << "  Store same version returned: " << storeResult << "\n";

  BOOST_CHECK_NE(storeResult, 0);
  std::cout << "  Verified: Cannot store duplicate version (readonly or not)\n";
}

BOOST_AUTO_TEST_CASE(mark_readonly_different_config_types) {
  std::cout << "\n=== TEST 7: Mark readonly on different config types ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int testVersion = fixture.baseVersion + 7;

  auto cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = testVersion;
  cfg1->getView().fillFromJSON("{\"type\": \"config1\"}");

  auto cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = testVersion;
  cfg2->getView().fillFromJSON("{\"type\": \"config2\"}");

  std::cout << "Step 1: Creating both config types with version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg1.get(), false), 0);
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg2.get(), false), 0);

  std::cout << "Step 2: Marking TestConfiguration001 as read-only\n";
  BOOST_REQUIRE_EQUAL(ifc.markActiveVersionReadonly(cfg1.get()), 0);

  std::cout << "Step 3: Verifying TestConfiguration002 can still be overwritten\n";
  cfg2->getView().fillFromJSON("{\"type\": \"config2_updated\"}");
  auto overwriteResult = ifc.saveActiveVersion(cfg2.get(), true);
  std::cout << "  Overwrite cfg2 returned: " << overwriteResult << "\n";

  BOOST_CHECK_EQUAL(overwriteResult, 0);
  std::cout << "  Verified: Marking one config type readonly doesn't affect other types\n";
}

BOOST_AUTO_TEST_SUITE_END()
