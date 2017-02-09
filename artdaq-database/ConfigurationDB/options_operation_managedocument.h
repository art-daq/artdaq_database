#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_READWRITE_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_READWRITE_H_

#include "artdaq-database/ConfigurationDB/options_operation_base.h"

#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

namespace artdaq {
namespace database {
namespace basictypes {
class JsonData;
}

namespace configuration {
namespace apiliteral = artdaq::database::configapi::literal;

using artdaq::database::basictypes::JsonData;
using artdaq::database::configuration::options::data_format_t;

class ManageDocumentOperation final : public OperationBase {
 public:
  ManageDocumentOperation(std::string const&);

  std::string const& version() const;
  std::string const& version(std::string const&);

  std::string const& sourceFileName() const;
  std::string const& sourceFileName(std::string const&);

  std::string const& entity() const;
  std::string const& entity(std::string const&);

  std::string const& run() const;
  std::string const& run(std::string const&);
  
  std::string const& configuration() const;
  std::string const& configuration(std::string const&);

  JsonData query_filter_to_JsonData() const override;

  JsonData configuration_to_JsonData() const;
  JsonData configurationsname_to_JsonData() const;
  JsonData version_to_JsonData() const;
  JsonData entity_to_JsonData() const;
  JsonData collection_to_JsonData() const;
  JsonData run_to_JsonData() const;

  bpo::options_description makeProgramOptions() const override;

  int readProgramOptions(bpo::variables_map const&) override;
  void readJsonData(JsonData const&) override;

  JsonData writeJsonData() const override;

 private:
  std::string _version = {apiliteral::notprovided};
  std::string _entity = {apiliteral::notprovided};
  std::string _run = {apiliteral::notprovided};
  std::string _configuration = {apiliteral::notprovided};
  std::string _source_file_name = {apiliteral::notprovided};
};

namespace debug {
namespace options {
void ManageDocuments();
}
}
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_READWRITE_H_ */
