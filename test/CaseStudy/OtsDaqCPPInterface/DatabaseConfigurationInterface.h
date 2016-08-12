#ifndef _ots_DatabaseConfigurationInterface_h_
#define _ots_DatabaseConfigurationInterface_h_

//#include "otsdaq-core/ConfigurationInterface/ConfigurationInterface.h"

#include "ConfigurationInterface.h"

#include <set>
#include <map>
#include <list>
#include <stdexcept>

namespace ots {

class ConfigurationBase;

class DatabaseConfigurationInterface : public ConfigurationInterface {
public:
  using config_version_map_t= std::map<std::string /*name*/,int /*version*/>;
DatabaseConfigurationInterface() { ; }
~DatabaseConfigurationInterface() { ; }

// read configuration from database
int fill(ConfigurationBase* /*configuration*/, int /*version*/) const noexcept;

// write configuration to database
int saveActiveVersion(const ConfigurationBase* /*configuration*/) const noexcept;

// find the latest configuration version by configuration type
int findLatestVersion(const ConfigurationBase* /*configuration*/) const noexcept;

// find all configuration versions by configuration type
std::set<int> getVersions(const ConfigurationBase* /*configuration*/) const noexcept;

// returns a list of all configuration data types
std::list<std::string /*name*/> listConfigurationsTypes() const throw(std::runtime_error);

// find all global configurations in database
std::list<std::string /*name*/> findAllGlobalConfigurations() const throw(std::runtime_error);

// return the contents of a global configuration
config_version_map_t loadGlobalConfiguration(
    std::string const& /*globalConfiguration*/) const throw(std::runtime_error);

// create a new global configuration from the contents map
void storeGlobalConfiguration(config_version_map_t const& /*configurationMap*/,
			      std::string const& /*globalConfiguration*/) const throw(std::runtime_error);

private:
};
}

#endif