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
class JsonData;
}

namespace configuration {

using artdaq::database::basictypes::JsonData;
using artdaq::database::configuration::options::data_format_t;

namespace apiliteral = artdaq::database::configapi::literal;

class OperationBase {
 public:
  OperationBase(std::string const& process_name);
  std::string const& operation() const noexcept;
  std::string const& operation(std::string const&);

  std::string const& collection() const noexcept;
  std::string const& collection(std::string const&);

  std::string const& provider() const noexcept;
  std::string const& provider(std::string const&);

  data_format_t const& format() const noexcept;
  data_format_t const& format(data_format_t const&);
  data_format_t const& format(std::string const&);

  std::string const& queryFilter() const noexcept;
  std::string const& queryFilter(std::string const&);

  virtual JsonData query_filter_to_JsonData() const;

  JsonData to_JsonData() const;
  std::string to_string() const;
  operator std::string() const;

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
};

std::ostream& operator<<(std::ostream&, OperationBase const&);

namespace debug {
namespace options {
void OperationBase();
}
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_BASE_H_ */
