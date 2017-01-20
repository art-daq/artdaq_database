#include "DatabaseConfigurationInterface.h"

#include <algorithm>
#include <iterator>

//#include "otsdaq-core/ConfigurationInterface/ConfigurationInterface.h"

#include "ConfigurationInterface.h"
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"

#include "artdaq-database/BasicTypes/basictypes.h"
using artdaq::database::basictypes::FhiclData;
using artdaq::database::basictypes::JsonData;

namespace artdaq {
namespace database {
namespace configuration {

using ots::ConfigurationBase;

template <>
template <>
bool MakeSerializable<ConfigurationBase const*>::writeDocumentImpl<JsonData>(JsonData& data) const {
  std::ostringstream oss;

  _conf->getView().printJSON(ss);

  data.json_buffer = oss.str();

  return true;
}

template <>
std::string MakeSerializable<ConfigurationBase const*>::configurationNameImpl() const {
  return _conf->getConfigurationName();
}

template <>
template <>
bool MakeSerializable<ConfigurationBase*>::readDocumentImpl<JsonData>(JsonData const& data) {
  int retVal = _conf->getViewP()->fillFromJSON(data.json_buffer);

  return (retVal >= 0);
}

template <>
std::string MakeSerializable<ConfigurationBase*>::configurationNameImpl() const {
  return _conf->getConfigurationName();
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
