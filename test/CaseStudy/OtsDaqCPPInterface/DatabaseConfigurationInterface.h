#ifndef _ots_DatabaseConfigurationInterface_h_
#define _ots_DatabaseConfigurationInterface_h_

// #include "otsdaq-core/ConfigurationInterface/ConfigurationInterface.h"

#include "ConfigurationInterface.h"

#include <list>
#include <map>
#include <set>
#include <stdexcept>

namespace ots {

struct ConfigurationBase;

class DatabaseConfigurationInterface : public ConfigurationInterface {
 public:
  using config_version_map_t = std::map<std::string /*name*/, int /*version*/>;
  DatabaseConfigurationInterface() { ; }
  ~DatabaseConfigurationInterface() { ; }

  // read configuration from database
  int fill(ConfigurationBase* /*configuration*/, int /*version*/) const noexcept;

  // write configuration to database
  int saveActiveVersion(const ConfigurationBase* /*configuration*/, bool /*overwrite*/ = false) const noexcept;

  // mark configuration as read-only in database
  int markActiveVersionReadonly(const ConfigurationBase* /*configuration*/) const noexcept;

  // find the latest configuration version by configuration type
  int findLatestVersion(const ConfigurationBase* /*configuration*/) const noexcept;

  // find all configuration versions by configuration type
  std::set<int> getVersions(const ConfigurationBase* /*configuration*/) const noexcept;

  // returns a set of all configuration data types
  std::set<std::string /*name*/> listConfigurationsTypes() const;

  // find all global configurations in database
  std::set<std::string /*name*/> findAllGlobalConfigurations(std::string const& /*search*/ = "") const;

  // return the contents of a global configuration
  config_version_map_t loadGlobalConfiguration(std::string const& /*configuration*/) const;

  // create a new global configuration from the contents map
  void storeGlobalConfiguration(config_version_map_t const& /*configurationMap*/, std::string const& /*configuration*/) const;

  // create a new global configuration from the contents map
  void storeGlobalConfiguration_mt(config_version_map_t const& /*configurationMap*/, std::string const& /*configuration*/) const;

 private:
};
}  // namespace ots

#endif
