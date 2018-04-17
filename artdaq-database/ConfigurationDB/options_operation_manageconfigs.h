#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_MANAGECONFIGS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_MANAGECONFIGS_H_

#include "artdaq-database/ConfigurationDB/options_operation_base.h"

#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

namespace artdaq {
namespace database {
namespace basictypes {
struct JsonData;
}

namespace configuration {
namespace apiliteral = artdaq::database::configapi::literal;

using artdaq::database::basictypes::JsonData;
using artdaq::database::configuration::options::data_format_t;

class ManageConfigsOperation final : public OperationBase {
 public:
  ManageConfigsOperation(std::string const&);

  std::string const& version() const;
  std::string const& version(std::string const&);

  std::string const& entity() const;
  std::string const& entity(std::string const&);

  std::string const& configuration() const;
  std::string const& configuration(std::string const&);

  JsonData query_filter_to_JsonData() const override;

  JsonData configuration_to_JsonData() const;
  JsonData version_to_JsonData() const;
  JsonData entity_to_JsonData() const;

  bpo::options_description makeProgramOptions() const override;

  int readProgramOptions(bpo::variables_map const&) override;
  void readJsonData(JsonData const&) override;

  JsonData writeJsonData() const override;

 private:
  std::string _version = {apiliteral::notprovided};
  std::string _entity = {apiliteral::notprovided};
  std::string _configuration = {apiliteral::notprovided};
};

namespace debug {
namespace options {
void ManageConfigs();
}
}  // namespace debug

}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_MANAGECONFIGS_H_ \
        */
