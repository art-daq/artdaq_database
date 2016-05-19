#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_BULKOPERATIONS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_BULKOPERATIONS_H_

#include <list>

#include "artdaq-database/ConfigurationDB/options_operation_base.h"

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

class BulkOperations final {
 public:
  using OperationBaseUPtr = std::unique_ptr<OperationBase>;
  using OperationsList = std::list<std::tuple<std::string, OperationBaseUPtr>>;
  
  using iterator = std::list<std::tuple<std::string, OperationBaseUPtr>>::iterator;
  using const_iterator = std::list<std::tuple<std::string, OperationBaseUPtr>>::const_iterator;
  
  BulkOperations(std::string const&);

  std::string const& bulkOperations() const noexcept;
  std::string const& bulkOperations(std::string const&);

  data_format_t const& dataFormat() const noexcept;

  JsonData to_JsonData() const;
  std::string to_string() const;

  virtual bpo::options_description makeProgramOptions() const;

  virtual int readProgramOptions(bpo::variables_map const&);
  virtual void readJsonData(JsonData const&);

  iterator begin() {return _operations_list.begin();}
  iterator end() {return _operations_list.end();}

  const_iterator begin() const {return _operations_list.cbegin();}
  const_iterator end() const {return _operations_list.cend();}
  
 private:
  std::string _process_name = {literal::notprovided};

  std::string _bulk_operations = {literal::notprovided};
  
  data_format_t _data_format = {data_format_t::gui};

  OperationsList _operations_list;
};

namespace debug {
namespace options {
void enableBulkOperations();
}
}
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_BULKOPERATIONS_H_ */
