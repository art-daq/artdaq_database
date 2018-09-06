#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_BASE_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_BASE_H_

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

namespace artdaq {
namespace database {
namespace basictypes {
struct JsonData;
}
namespace docrecord {
class JSONDocument;
}

namespace configuration {

using artdaq::database::basictypes::JsonData;
using artdaq::database::configuration::options::data_format_t;
using artdaq::database::docrecord::JSONDocument;

namespace apiliteral = artdaq::database::configapi::literal;
namespace jsonliteral = artdaq::database::dataformats::literal;

class OperationBase {
 public:
  OperationBase(std::string process_name);
  virtual ~OperationBase();

  std::string const& operation() const;
  std::string const& operation(std::string const&);

  std::string const& collection() const;
  std::string const& collection(std::string const&);

  std::string const& provider() const;
  std::string const& provider(std::string const&);

  data_format_t const& format() const;
  data_format_t const& format(data_format_t const&);
  data_format_t const& format(std::string const&);

  std::string const& queryFilter() const;
  std::string const& queryFilter(std::string const&);

  std::string const& processName() const;

  std::string const& resultFileName() const;
  std::string const& resultFileName(std::string const&);

  virtual JsonData query_filter_to_JsonData() const;

  JsonData to_JsonData() const;
  std::string to_string() const;
  operator std::string() const;

  operator JSONDocument() const;

  virtual bpo::options_description makeProgramOptions() const;

  virtual int readProgramOptions(bpo::variables_map const&);
  virtual void readJsonData(JsonData const&);

  virtual JsonData writeJsonData() const;

 private:
  std::string _getProviderFromURI();

 private:
  std::string _process_name = {apiliteral::notprovided};

  std::string _provider = {apiliteral::provider::filesystem};
  std::string _operation = {apiliteral::operation::readdocument};
  data_format_t _data_format = {data_format_t::unknown};
  std::string _collection_name = {apiliteral::notprovided};
  std::string _query_payload = {apiliteral::notprovided};

  std::string _result_file_name = {apiliteral::notprovided};
};

std::ostream& operator<<(std::ostream&, OperationBase const&);

namespace debug {
namespace options {
void OperationBase();
}
}  // namespace debug

}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_BASE_H_ */
