#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (ListCollections test)

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <set>
#include <string>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct ListCollectionsTestData {
  ListCollectionsTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 400000;
    std::cout << "=== LIST COLLECTIONS TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~ListCollectionsTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

ListCollectionsTestData fixture;

BOOST_AUTO_TEST_SUITE(list_collections_test)

BOOST_AUTO_TEST_CASE(list_returns_created_types) {
  std::cout << "\n=== TEST 1: List collections returns created types ===\n";

  auto ifc = DatabaseConfigurationInterface();

  int testVersion = fixture.baseVersion + 1;

  auto cfg1 = std::make_shared<TestConfiguration001>();
  cfg1->getView().version = testVersion;
  cfg1->getView().fillFromJSON("{\"type\": \"config1\"}");

  auto cfg2 = std::make_shared<TestConfiguration002>();
  cfg2->getView().version = testVersion;
  cfg2->getView().fillFromJSON("{\"type\": \"config2\"}");

  std::cout << "Step 1: Creating TestConfiguration001 with version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg1.get(), false), 0);

  std::cout << "Step 2: Creating TestConfiguration002 with version " << testVersion << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg2.get(), false), 0);

  std::cout << "Step 3: Listing all configuration types\n";
  auto collections = ifc.listConfigurationsTypes();
  std::cout << "  Found " << collections.size() << " configuration types:\n";
  for (const auto& name : collections) {
    std::cout << "    - " << name << "\n";
  }

  std::cout << "Step 4: Verifying both types are present\n";
  bool found1 = collections.find(cfg1->getConfigurationName()) != collections.end();
  bool found2 = collections.find(cfg2->getConfigurationName()) != collections.end();

  std::cout << "  TestConfiguration001 present: " << (found1 ? "YES" : "NO") << "\n";
  std::cout << "  TestConfiguration002 present: " << (found2 ? "YES" : "NO") << "\n";

  BOOST_CHECK(found1);
  BOOST_CHECK(found2);
  std::cout << "  Verified: Both configuration types appear in list\n";
}

BOOST_AUTO_TEST_CASE(list_after_multiple_versions) {
  std::cout << "\n=== TEST 2: List after storing multiple versions ===\n";

  auto ifc = DatabaseConfigurationInterface();

  auto cfg = std::make_shared<TestConfiguration001>();

  std::cout << "Step 1: Storing 3 different versions of TestConfiguration001\n";
  for (int i = 0; i < 3; ++i) {
    int ver = fixture.baseVersion + 10 + i;
    cfg->getView().version = ver;
    cfg->getView().fillFromJSON("{\"iteration\": " + std::to_string(i) + "}");
    BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(cfg.get(), false), 0);
    std::cout << "  Stored version " << ver << "\n";
  }

  std::cout << "Step 2: Listing configuration types\n";
  auto collections = ifc.listConfigurationsTypes();

  std::cout << "Step 3: Verifying TestConfiguration001 appears exactly once\n";
  int count = 0;
  for (const auto& name : collections) {
    if (name == cfg->getConfigurationName()) {
      count++;
    }
  }
  std::cout << "  TestConfiguration001 appears " << count << " time(s)\n";

  BOOST_CHECK_EQUAL(count, 1);
  std::cout << "  Verified: Config type appears once regardless of version count\n";
}

BOOST_AUTO_TEST_CASE(list_is_consistent) {
  std::cout << "\n=== TEST 3: List is consistent across calls ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::cout << "Step 1: Calling listConfigurationsTypes twice\n";
  auto collections1 = ifc.listConfigurationsTypes();
  auto collections2 = ifc.listConfigurationsTypes();

  std::cout << "  First call returned " << collections1.size() << " types\n";
  std::cout << "  Second call returned " << collections2.size() << " types\n";

  BOOST_CHECK_EQUAL(collections1.size(), collections2.size());

  std::cout << "Step 2: Verifying both calls return same types\n";
  bool identical = (collections1 == collections2);
  std::cout << "  Results identical: " << (identical ? "YES" : "NO") << "\n";

  BOOST_CHECK(identical);
  std::cout << "  Verified: listConfigurationsTypes is consistent\n";
}

BOOST_AUTO_TEST_CASE(list_after_new_type_added) {
  std::cout << "\n=== TEST 4: List updates after new type added ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::cout << "Step 1: Getting initial list\n";
  auto initialCollections = ifc.listConfigurationsTypes();
  size_t initialCount = initialCollections.size();
  std::cout << "  Initial count: " << initialCount << "\n";

  struct UniqueConfigForList final : public ConfigurationBase {
    UniqueConfigForList(int suffix) : _suffix(suffix) {}
    std::string getConfigurationName() const { return "UniqueListTest_" + std::to_string(_suffix); }
    ConfigurationView& getView() { return view; }
    ConfigurationView const& getView() const { return view; }
    ConfigurationView* getViewP() { return &view; }
    int getViewVersion() const { return view.version; }
    ConfigurationView view;
    int _suffix;
  };

  int uniqueSuffix = rand() % 1000000;
  auto uniqueCfg = std::make_shared<UniqueConfigForList>(uniqueSuffix);
  uniqueCfg->getView().version = fixture.baseVersion + 20;
  uniqueCfg->getView().fillFromJSON("{\"unique\": true}");

  std::cout << "Step 2: Adding new unique config type: " << uniqueCfg->getConfigurationName() << "\n";
  BOOST_REQUIRE_EQUAL(ifc.saveActiveVersion(uniqueCfg.get(), false), 0);

  std::cout << "Step 3: Getting updated list\n";
  auto updatedCollections = ifc.listConfigurationsTypes();
  size_t updatedCount = updatedCollections.size();
  std::cout << "  Updated count: " << updatedCount << "\n";

  std::cout << "Step 4: Verifying new type appears in list\n";
  bool foundNew = updatedCollections.find(uniqueCfg->getConfigurationName()) != updatedCollections.end();
  std::cout << "  New type found: " << (foundNew ? "YES" : "NO") << "\n";

  BOOST_CHECK(foundNew);
  BOOST_CHECK_GE(updatedCount, initialCount);
  std::cout << "  Verified: New configuration type appears in list after creation\n";
}

BOOST_AUTO_TEST_CASE(list_returns_set_type) {
  std::cout << "\n=== TEST 5: List returns proper set (no duplicates) ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::cout << "Step 1: Getting configuration types list\n";
  auto collections = ifc.listConfigurationsTypes();

  std::cout << "Step 2: Checking for duplicates\n";
  std::set<std::string> uniqueNames;
  bool hasDuplicates = false;
  for (const auto& name : collections) {
    if (uniqueNames.find(name) != uniqueNames.end()) {
      std::cout << "  DUPLICATE FOUND: " << name << "\n";
      hasDuplicates = true;
    }
    uniqueNames.insert(name);
  }

  std::cout << "  Total items: " << collections.size() << "\n";
  std::cout << "  Unique items: " << uniqueNames.size() << "\n";

  BOOST_CHECK(!hasDuplicates);
  BOOST_CHECK_EQUAL(collections.size(), uniqueNames.size());
  std::cout << "  Verified: List contains no duplicates\n";
}

BOOST_AUTO_TEST_CASE(list_handles_special_names) {
  std::cout << "\n=== TEST 6: List handles various config names ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::cout << "Step 1: Verifying standard config names are handled\n";
  auto collections = ifc.listConfigurationsTypes();

  for (const auto& name : collections) {
    BOOST_CHECK(!name.empty());
    std::cout << "  Valid name: " << name << " (length: " << name.length() << ")\n";
  }

  std::cout << "  Verified: All configuration names are non-empty strings\n";
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
