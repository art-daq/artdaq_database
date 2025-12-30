#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (SafeWrappers test)

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct TestFixture {
  TestFixture() {
    std::cout << "SafeWrappers test fixture setup\n";
    std::cout << "ARTDAQ_DATABASE_URI=" << (getenv("ARTDAQ_DATABASE_URI") != nullptr ? getenv("ARTDAQ_DATABASE_URI") : std::string("notprovided"))
              << "\n";
  }

  ~TestFixture() { std::cout << "SafeWrappers test fixture teardown\n"; }

  const int _version = (srand(time(nullptr)), rand() % 99999 + 100000);

  int version() const { return _version; }
};

TestFixture fixture;

BOOST_AUTO_TEST_SUITE(safe_wrappers_test)

BOOST_AUTO_TEST_CASE(getVersions_safe_success) {
  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  auto ifc = DatabaseConfigurationInterface();

  cfg->getView().version = fixture.version();
  auto storeResult = ifc.saveActiveVersion(cfg.get());
  BOOST_CHECK_EQUAL(storeResult, 0);

  std::set<int> versions;
  auto result = ifc.getVersions_safe(cfg.get(), versions);

  BOOST_CHECK_EQUAL(result.first, true);
  BOOST_CHECK_MESSAGE(!versions.empty(), "Versions set should not be empty");

  auto found = (versions.find(fixture.version()) != versions.end());
  BOOST_CHECK_MESSAGE(found, "Stored version should be in the versions set");
}

BOOST_AUTO_TEST_CASE(getVersions_safe_multiple_calls) {
  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  auto ifc = DatabaseConfigurationInterface();

  std::set<int> versions1, versions2;

  auto result1 = ifc.getVersions_safe(cfg.get(), versions1);
  auto result2 = ifc.getVersions_safe(cfg.get(), versions2);

  BOOST_CHECK_EQUAL(result1.first, true);
  BOOST_CHECK_EQUAL(result2.first, true);
  BOOST_CHECK(versions1 == versions2);
}

BOOST_AUTO_TEST_CASE(listConfigurationsTypes_safe_success) {
  auto ifc = DatabaseConfigurationInterface();

  std::set<std::string> collections;
  auto result = ifc.listConfigurationsTypes_safe(collections);

  BOOST_CHECK_EQUAL(result.first, true);
  std::cout << "Found " << collections.size() << " configuration types\n";
  for (auto const& c : collections) {
    std::cout << "  - " << c << "\n";
  }
}

BOOST_AUTO_TEST_CASE(findAllGlobalConfigurations_safe_setup) {
  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();

  auto ifc = DatabaseConfigurationInterface();

  cfg1->getView().version = fixture.version() + 100;
  cfg2->getView().version = fixture.version() + 200;

  ifc.saveActiveVersion(cfg1.get());
  ifc.saveActiveVersion(cfg2.get());

  auto map = DatabaseConfigurationInterface::config_version_map_t{};
  map[cfg1->getConfigurationName()] = cfg1->getView().version;
  map[cfg2->getConfigurationName()] = cfg2->getView().version;

  auto configName = std::string{"safewrapper_config_"} + std::to_string(fixture.version());
  ifc.storeGlobalConfiguration(map, configName);

  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(findAllGlobalConfigurations_safe_success) {
  auto ifc = DatabaseConfigurationInterface();

  std::set<std::string> configurations;
  auto result = ifc.findAllGlobalConfigurations_safe("", configurations);

  BOOST_CHECK_EQUAL(result.first, true);
  BOOST_CHECK_MESSAGE(!configurations.empty(), "Should find at least one global configuration");

  std::cout << "Found " << configurations.size() << " global configurations\n";
}

BOOST_AUTO_TEST_CASE(findAllGlobalConfigurations_safe_with_filter) {
  auto ifc = DatabaseConfigurationInterface();
  auto configName = std::string{"safewrapper_config_"} + std::to_string(fixture.version());

  std::set<std::string> configurations;
  auto result = ifc.findAllGlobalConfigurations_safe(configName, configurations);

  BOOST_CHECK_EQUAL(result.first, true);

  auto found = (configurations.find(configName) != configurations.end());
  BOOST_CHECK_MESSAGE(found, "Should find the specific global configuration");
}

BOOST_AUTO_TEST_CASE(loadGlobalConfiguration_safe_success) {
  auto ifc = DatabaseConfigurationInterface();
  auto configName = std::string{"safewrapper_config_"} + std::to_string(fixture.version());

  DatabaseConfigurationInterface::config_version_map_t configMap;
  auto result = ifc.loadGlobalConfiguration_safe(configName, configMap);

  BOOST_CHECK_EQUAL(result.first, true);
  BOOST_CHECK_EQUAL(configMap.size(), 2);

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  std::shared_ptr<ConfigurationBase> cfg2 = std::make_shared<TestConfiguration002>();

  BOOST_CHECK_EQUAL(configMap.at(cfg1->getConfigurationName()), fixture.version() + 100);
  BOOST_CHECK_EQUAL(configMap.at(cfg2->getConfigurationName()), fixture.version() + 200);
}

BOOST_AUTO_TEST_CASE(loadGlobalConfiguration_safe_nonexistent) {
  auto ifc = DatabaseConfigurationInterface();
  auto configName = std::string{"nonexistent_config_"} + std::to_string(fixture.version());

  DatabaseConfigurationInterface::config_version_map_t configMap;
  auto result = ifc.loadGlobalConfiguration_safe(configName, configMap);

  BOOST_CHECK_EQUAL(result.first, false);
  BOOST_CHECK_MESSAGE(!result.second.empty(), "Error message should not be empty");
  BOOST_CHECK_MESSAGE(configMap.empty(), "Config map should be empty on failure");

  std::cout << "Expected error for non-existent config: " << result.second << "\n";
}

BOOST_AUTO_TEST_CASE(findCompositionsContaining_safe_success) {
  auto ifc = DatabaseConfigurationInterface();

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  auto versionStr = std::to_string(fixture.version() + 100);

  std::set<std::string> compositions;
  auto result = ifc.findCompositionsContaining_safe(cfg1->getConfigurationName(), versionStr, compositions);

  BOOST_CHECK_EQUAL(result.first, true);

  auto configName = std::string{"safewrapper_config_"} + std::to_string(fixture.version());
  auto found = (compositions.find(configName) != compositions.end());
  BOOST_CHECK_MESSAGE(found, "Should find the composition containing the version");

  std::cout << "Found " << compositions.size() << " compositions containing version " << versionStr << "\n";
}

BOOST_AUTO_TEST_CASE(findCompositionsContaining_safe_not_found) {
  auto ifc = DatabaseConfigurationInterface();

  std::shared_ptr<ConfigurationBase> cfg1 = std::make_shared<TestConfiguration001>();
  auto versionStr = std::to_string(999999);

  std::set<std::string> compositions;
  auto result = ifc.findCompositionsContaining_safe(cfg1->getConfigurationName(), versionStr, compositions);

  BOOST_CHECK_EQUAL(result.first, true);
  BOOST_CHECK_MESSAGE(compositions.empty(), "Should not find any compositions for non-existent version");
}

BOOST_AUTO_TEST_CASE(compare_safe_and_throwing_getVersions) {
  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  auto ifc = DatabaseConfigurationInterface();

  std::set<int> throwingVersions;
  try {
    throwingVersions = ifc.getVersions(cfg.get());
  } catch (...) {
    throwingVersions.clear();
  }

  std::set<int> safeVersions;
  auto result = ifc.getVersions_safe(cfg.get(), safeVersions);

  BOOST_CHECK_EQUAL(result.first, true);
  BOOST_CHECK_EQUAL(throwingVersions.size(), safeVersions.size());
  BOOST_CHECK(throwingVersions == safeVersions);
}

BOOST_AUTO_TEST_CASE(compare_safe_and_throwing_findGlobalConfigurations) {
  auto ifc = DatabaseConfigurationInterface();

  std::set<std::string> throwingConfigs;
  try {
    throwingConfigs = ifc.findAllGlobalConfigurations("");
  } catch (...) {
    throwingConfigs.clear();
  }

  std::set<std::string> safeConfigs;
  auto result = ifc.findAllGlobalConfigurations_safe("", safeConfigs);

  BOOST_CHECK_EQUAL(result.first, true);
  BOOST_CHECK_EQUAL(throwingConfigs.size(), safeConfigs.size());
  BOOST_CHECK(throwingConfigs == safeConfigs);
}

BOOST_AUTO_TEST_CASE(compare_safe_and_throwing_loadGlobalConfiguration) {
  auto ifc = DatabaseConfigurationInterface();
  auto configName = std::string{"safewrapper_config_"} + std::to_string(fixture.version());

  DatabaseConfigurationInterface::config_version_map_t throwingMap;
  try {
    throwingMap = ifc.loadGlobalConfiguration(configName);
  } catch (...) {
    throwingMap.clear();
  }

  DatabaseConfigurationInterface::config_version_map_t safeMap;
  auto result = ifc.loadGlobalConfiguration_safe(configName, safeMap);

  BOOST_CHECK_EQUAL(result.first, true);
  BOOST_CHECK_EQUAL(throwingMap.size(), safeMap.size());
  BOOST_CHECK(throwingMap == safeMap);
}

BOOST_AUTO_TEST_SUITE_END()
