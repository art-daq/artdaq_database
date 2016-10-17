#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_BASE_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_BASE_H_

#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/ConfigurationDB/shared_literals.h"

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

class OperationBase {
 public:
  OperationBase(std::string const& process_name);
  std::string const& operation() const noexcept;
  std::string const& operation(std::string const&);

  std::string const& collectionName() const noexcept;
  std::string const& collectionName(std::string const&);

  std::string const& provider() const noexcept;
  std::string const& provider(std::string const&);

  data_format_t const& dataFormat() const noexcept;
  data_format_t const& dataFormat(data_format_t const&);
  data_format_t const& dataFormat(std::string const&);

  std::string const& searchFilter() const noexcept;
  std::string const& searchFilter(std::string const&);

  virtual JsonData search_filter_to_JsonData() const;

  JsonData to_JsonData() const;
  std::string to_string() const;

  virtual bpo::options_description makeProgramOptions() const;

  virtual int readProgramOptions(bpo::variables_map const&);
  virtual void readJsonData(JsonData const&);

  virtual JsonData writeJsonData() const;

 private:
  std::string _getProviderFromURI();
  
 private:  
  std::string _process_name = {literal::notprovided};

  std::string _provider = {literal::database_provider_filesystem};
  std::string _operation = {literal::operation::load};
  data_format_t _data_format = {data_format_t::unknown};
  std::string _collection_name = {literal::notprovided};
  std::string _search_filter = {literal::notprovided};
};

namespace debug {
namespace options {
void enableOperationBase();
}
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_BASE_H_ */
