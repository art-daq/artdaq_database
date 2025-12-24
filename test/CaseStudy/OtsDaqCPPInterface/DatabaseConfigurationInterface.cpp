#include "DatabaseConfigurationInterface.h"

#include <algorithm>
#include <iterator>

#include "artdaq-database/ConfigurationDB/configurationdbifc.h"

#include "artdaq-database/BasicTypes/basictypes.h"
using artdaq::database::basictypes::JsonData;

using ots::DatabaseConfigurationInterface;
using config_version_map_t = ots::DatabaseConfigurationInterface::config_version_map_t;

namespace db = artdaq::database::configuration;
using VersionInfoList_t = db::ConfigurationInterface::VersionInfoList_t;

constexpr auto default_dbprovider = "filesystem";
constexpr auto default_entity = "OTSDAQROOT";

int DatabaseConfigurationInterface::fill(ConfigurationBase* configuration, int version) const noexcept {
  auto ifc = db::ConfigurationInterface{default_dbprovider};

  auto versionstring = std::to_string(version);

  if (-1 == version) {
    versionstring = std::to_string(findLatestVersion(configuration));
  }

  auto result = ifc.template loadVersion<decltype(configuration), JsonData>(configuration, versionstring, default_entity);

  if (result.first) {
    return 0;
  }

  std::cout << "DBI Error:" << result.second << "\n";

  return -1;
}

int DatabaseConfigurationInterface::saveActiveVersion(const ConfigurationBase* configuration, bool overwrite) const noexcept {
  auto ifc = db::ConfigurationInterface{default_dbprovider};

  auto versionstring = std::to_string(configuration->getViewVersion());

  auto result = overwrite ? ifc.template overwriteVersion<decltype(configuration), JsonData>(configuration, versionstring, default_entity)
                          : ifc.template storeVersion<decltype(configuration), JsonData>(configuration, versionstring, default_entity);

  if (result.first) {
    return 0;
  }

  std::cout << "DBI Error:" << result.second << "\n";

  return -1;
}

int DatabaseConfigurationInterface::markActiveVersionReadonly(const ConfigurationBase* configuration) const noexcept {
  auto ifc = db::ConfigurationInterface{default_dbprovider};

  auto versionstring = std::to_string(configuration->getViewVersion());

  auto result = ifc.template markVersionReadonly<decltype(configuration), JsonData>(configuration, versionstring, default_entity);

  if (result.first) {
    return 0;
  }

  std::cout << "DBI Error:" << result.second << "\n";

  return -1;
}

int DatabaseConfigurationInterface::findLatestVersion(const ConfigurationBase* configuration) const noexcept {
  auto versions = getVersions(configuration);

  if (versions.empty()) {
    return -1;
  }

  return *(versions.rbegin());
}

std::set<int> DatabaseConfigurationInterface::getVersions(const ConfigurationBase* configuration) const noexcept try {
  auto ifc = db::ConfigurationInterface{default_dbprovider};
  auto result = ifc.template getVersions<decltype(configuration)>(configuration, default_entity);

  auto to_set = [](auto const& inputList) {
    auto resultSet = std::set<int>{};
    std::for_each(inputList.begin(), inputList.end(),
                  [&resultSet](std::string const& version) { resultSet.insert(std::stol(version, nullptr, 10)); });
    return resultSet;
  };

  return to_set(result);
} catch (std::exception const& e) {
  std::cout << "DBI Exception:" << e.what() << "\n";
  return {};
}

std::set<std::string> DatabaseConfigurationInterface::listConfigurationsTypes() const try {
  auto ifc = db::ConfigurationInterface{default_dbprovider};

  auto collection_name_prefix = std::string{};

  return ifc.listCollections(collection_name_prefix);
} catch (std::exception const& e) {
  std::cout << "DBI Exception:" << e.what() << "\n";
  throw std::runtime_error(e.what());
} catch (...) {
  std::cout << "DBI Unknown exception.\n";
  throw std::runtime_error("DBI Unknown exception.");
}

std::set<std::string> DatabaseConfigurationInterface::findAllGlobalConfigurations(std::string const& search) const try {
  auto ifc = db::ConfigurationInterface{default_dbprovider};

  return ifc.findGlobalConfigurations(search);
} catch (std::exception const& e) {
  std::cout << "DBI Exception:" << e.what() << "\n";
  throw std::runtime_error(e.what());
} catch (...) {
  std::cout << "DBI Unknown exception.\n";
  throw std::runtime_error("DBI Unknown exception.");
}

config_version_map_t DatabaseConfigurationInterface::loadGlobalConfiguration(std::string const& configuration) const try {
  auto ifc = db::ConfigurationInterface{default_dbprovider};
  auto result = ifc.loadGlobalConfiguration(configuration);

  auto to_map = [](auto const& inputList) {
    auto resultMap = config_version_map_t{};

    std::for_each(inputList.begin(), inputList.end(),
                  [&resultMap](auto const& info) { resultMap[info.configuration] = std::stol(info.version, 0, 10); });

    return resultMap;
  };

  return to_map(result);
} catch (std::exception const& e) {
  std::cout << "DBI Exception:" << e.what() << "\n";
  throw std::runtime_error(e.what());
} catch (...) {
  std::cout << "DBI Unknown exception.\n";
  throw std::runtime_error("DBI Unknown exception.");
}

namespace {
VersionInfoList_t to_version_info_list(config_version_map_t const& inputMap) {
  auto resultList = VersionInfoList_t{};
  std::transform(inputMap.begin(), inputMap.end(), std::back_inserter(resultList),
                 [](auto const& mapEntry) { return VersionInfoList_t::value_type{mapEntry.first, std::to_string(mapEntry.second), default_entity}; });
  return resultList;
}
}  // namespace

void DatabaseConfigurationInterface::storeGlobalConfiguration(config_version_map_t const& configurationMap, std::string const& configuration,
                                                              bool allowOverwrite) const try {
  auto ifc = db::ConfigurationInterface{default_dbprovider};

  auto result = ifc.storeGlobalConfiguration(to_version_info_list(configurationMap), configuration, allowOverwrite);

  if (result.first) {
    return;
  }

  throw std::runtime_error(result.second);
} catch (std::exception const& e) {
  std::cout << "DBI Exception:" << e.what() << "\n";
  throw std::runtime_error(e.what());
} catch (...) {
  std::cout << "DBI Unknown exception.\n";
  throw std::runtime_error("DBI Unknown exception.");
}

void DatabaseConfigurationInterface::storeGlobalConfiguration_mt(const config_version_map_t& configurationMap, const std::string& configuration,
                                                                 bool allowOverwrite) const try {
  auto ifc = db::ConfigurationInterface{default_dbprovider};

  auto result = ifc.storeGlobalConfiguration_mt(to_version_info_list(configurationMap), configuration, allowOverwrite);

  if (result.first) {
    return;
  }

  throw std::runtime_error(result.second);
} catch (std::exception const& e) {
  std::cout << "DBI Exception:" << e.what() << "\n";
  throw std::runtime_error(e.what());
} catch (...) {
  std::cout << "DBI Unknown exception.\n";
  throw std::runtime_error("DBI Unknown exception.");
}

std::set<std::string> DatabaseConfigurationInterface::findCompositionsContaining(std::string const& configurationType,
                                                                                 std::string const& version) const try {
  auto ifc = db::ConfigurationInterface{default_dbprovider};

  return ifc.findGlobalConfigurationsContaining(configurationType, version);

} catch (std::exception const& e) {
  std::cout << "DBI Exception:" << e.what() << "\n";
  throw std::runtime_error(e.what());
} catch (...) {
  std::cout << "DBI Unknown exception.\n";
  throw std::runtime_error("DBI Unknown exception.");
}
