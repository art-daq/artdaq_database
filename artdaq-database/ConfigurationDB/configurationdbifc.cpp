#include "artdaq-database/ConfigurationDB/configurationdbifc.h"
#include <boost/exception/diagnostic_information.hpp>

struct ConfigurationBase {
  std::string getConfigurationName() const { return "ConfigurationBase"; }

  int aa1;
};

using artdaq::database::basictypes::FhiclData;
using artdaq::database::basictypes::JsonData;

namespace artdaq {
namespace database {
namespace configuration {

template <>
template <>
bool MakeSerializable<ConfigurationBase *>::writeConfigurationImpl<FhiclData>(
    FhiclData &data) const {
  std::stringstream ss;

  // configuration->getView().printJSON(ss);
  data.fhicl_buffer = ss.str();

  return true;
}

template <>
template <>
bool MakeSerializable<ConfigurationBase &>::writeConfigurationImpl<FhiclData>(
    FhiclData &data) const {

  std::stringstream ss;

  // configuration->getView().printJSON(ss);
  data.fhicl_buffer = ss.str();

  return true;
}

template <>
template <>
bool MakeSerializable<ConfigurationBase *>::readConfigurationImpl<JsonData>(
    JsonData const &data[[gnu::unused]]) {
  int retVal = -1; // configuration->getViewP()->fillFromJSON(data.json_buffer);

  return (retVal >= 0);
}

template <>
template <>
bool MakeSerializable<ConfigurationBase &>::readConfigurationImpl<JsonData>(
    JsonData const &data[[gnu::unused]]) {
  int retVal = -1; // configuration->getViewP()->fillFromJSON(data.json_buffer);

  return (retVal >= 0);
}

template <>
std::string
MakeSerializable<ConfigurationBase &>::configurationNameImpl() const {
  return _conf.getConfigurationName();
}

template <>
std::string
MakeSerializable<ConfigurationBase *>::configurationNameImpl() const {
  return _conf->getConfigurationName();
}

} // namespace configuration
} // namespace database
} // namespace artdaq

void testAAA() {
  namespace db = artdaq::database::configuration;

  auto conf1 = new ConfigurationBase{1};

  auto conf = ConfigurationBase{1};
  auto &conf2 = conf;

  auto ifc1 = db::ConfigurationInterface<decltype(conf1)>{"filesystem"};
  auto result1 = ifc1.template storeVersion<JsonData>(conf1, "12", "AA");

  auto ifc2 = db::ConfigurationInterface<decltype(conf2)>{"filesystem"};
  auto result2 = ifc2.template loadVersion<FhiclData>(conf2, "12", "AA");
}
