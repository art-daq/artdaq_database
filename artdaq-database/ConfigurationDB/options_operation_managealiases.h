#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_MANAGEALIASES_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_MANAGEALIASES_H_

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

class ManageAliasesOperation final : public OperationBase {
 public:
  ManageAliasesOperation(std::string const&);

  std::string const& version() const;
  std::string const& version(std::string const&);

  std::string const& versionAlias() const;
  std::string const& versionAlias(std::string const&);

  std::string const& entity() const;
  std::string const& entity(std::string const&);

  std::string const& run() const;
  std::string const& run(std::string const&);

  std::string const& configuration() const;
  std::string const& configuration(std::string const&);

  std::string const& configurationAlias() const;
  std::string const& configurationAlias(std::string const&);

  JsonData query_filter_to_JsonData() const override;

  JsonData versionAlias_to_JsonData() const;
  JsonData run_to_JsonData() const;

  bpo::options_description makeProgramOptions() const override;

  int readProgramOptions(bpo::variables_map const&) override;
  void readJsonData(JsonData const&) override;

  JsonData writeJsonData() const override;

 private:
  std::string _version = {apiliteral::notprovided};
  std::string _version_alias = {apiliteral::notprovided};

  std::string _entity = {apiliteral::notprovided};

  std::string _configuration = {apiliteral::notprovided};
  std::string _configuration_alias = {apiliteral::notprovided};

  std::string _run = {apiliteral::notprovided};
};

namespace debug {
namespace options {
void ManageAliases();
}
}  // namespace debug

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
namespace {
template <>
inline TraceStreamer& TraceStreamer::operator<<(const artdaq::database::configuration::ManageAliasesOperation& r) {
  std::ostringstream s;
  s << r.to_string();
  msg_append(s.str().c_str());
  return *this;
}
}  // namespace
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_MANAGEALIASES_H_ \
        */
