#ifndef _ots_DatabaseConfigurationInterface_h_
#define _ots_DatabaseConfigurationInterface_h_

#include "ConfigurationInterface.h"

#include <list>
#include <map>
#include <set>
#include <stdexcept>

namespace ots {

struct ConfigurationBase;

class DatabaseConfigurationInterface : public ConfigurationInterface {
 public:
  using config_version_map_t = std::map<std::string, int>;
  DatabaseConfigurationInterface() { ; }
  ~DatabaseConfigurationInterface() { ; }

  int fill(ConfigurationBase*, int) const noexcept;

  int saveActiveVersion(const ConfigurationBase*, bool = false) const noexcept;

  int markActiveVersionReadonly(const ConfigurationBase*) const noexcept;

  int findLatestVersion(const ConfigurationBase*) const noexcept;

  std::set<int> getVersions(const ConfigurationBase*) const noexcept;

  std::set<std::string> listConfigurationsTypes() const;

  std::set<std::string> findAllGlobalConfigurations(std::string const& = "") const;

  config_version_map_t loadGlobalConfiguration(std::string const&) const;

  void storeGlobalConfiguration(config_version_map_t const&, std::string const&, bool = false) const;

  void storeGlobalConfiguration_mt(config_version_map_t const&, std::string const&, bool = false) const;

  std::set<std::string> findCompositionsContaining(std::string const&, std::string const&) const;

  using result_t = std::pair<bool, std::string>;

  result_t getVersions_safe(const ConfigurationBase*, std::set<int>&) const noexcept;

  result_t listConfigurationsTypes_safe(std::set<std::string>&) const noexcept;

  result_t findAllGlobalConfigurations_safe(std::string const&, std::set<std::string>&) const noexcept;

  result_t loadGlobalConfiguration_safe(std::string const&, config_version_map_t&) const noexcept;

  result_t findCompositionsContaining_safe(std::string const&, std::string const&, std::set<std::string>&) const noexcept;

 private:
};
}  // namespace ots

#endif
