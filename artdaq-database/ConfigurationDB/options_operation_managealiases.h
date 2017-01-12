#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_MANAGEALIASES_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_MANAGEALIASES_H_

#include "artdaq-database/ConfigurationDB/options_operation_base.h"

#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

namespace artdaq {
namespace database {
namespace basictypes {
class JsonData;
}

namespace configuration {
namespace jsonliteral = artdaq::database::dataformats::literal;
namespace apiliteral = artdaq::database::configapi::literal;

using artdaq::database::basictypes::JsonData;
using artdaq::database::configuration::options::data_format_t;

class ManageAliasesOperation final : public OperationBase {
 public:
  ManageAliasesOperation(std::string const&);

  std::string const& version() const noexcept;
  std::string const& version(std::string const&);

  std::string const& versionAlias() const noexcept;
  std::string const& versionAlias(std::string const&);

  std::string const& configurableEntity() const noexcept;
  std::string const& configurableEntity(std::string const&);

  std::string const& globalConfiguration() const noexcept;
  std::string const& globalConfiguration(std::string const&);

  std::string const& globalConfigurationAlias() const noexcept;
  std::string const& globalConfigurationAlias(std::string const&);

  JsonData search_filter_to_JsonData() const override;

  bpo::options_description makeProgramOptions() const override;

  int readProgramOptions(bpo::variables_map const&) override;
  void readJsonData(JsonData const&) override;

  JsonData writeJsonData() const override;

 private:
  std::string _version = {jsonliteral::notprovided};
  std::string _version_alias = {jsonliteral::notprovided};

  std::string _configurable_entity = {jsonliteral::notprovided};

  std::string _global_configuration = {jsonliteral::notprovided};
  std::string _global_configuration_alias = {jsonliteral::notprovided};
};

namespace debug {
namespace options {
void enableOperationManageAliases();
}
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_MANAGEALIASES_H_ \
          */
