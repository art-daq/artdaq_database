#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_BULKOPERATIONS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_BULKOPERATIONS_H_

#include <list>

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

class BulkOperations final {
 public:
  using OperationBaseUPtr = std::unique_ptr<OperationBase>;
  using OperationsList = std::list<std::tuple<std::string, OperationBaseUPtr>>;

  using iterator = std::list<std::tuple<std::string, OperationBaseUPtr>>::iterator;
  using const_iterator = std::list<std::tuple<std::string, OperationBaseUPtr>>::const_iterator;

  BulkOperations(std::string);

  std::string const& bulkOperations() const;
  std::string const& bulkOperations(std::string const&);

  data_format_t const& format() const;

  JsonData to_JsonData() const;
  std::string to_string() const;

  bpo::options_description makeProgramOptions() const;

  int readProgramOptions(bpo::variables_map const&);
  void readJsonData(JsonData const&);

  iterator begin() { return _operations_list.begin(); }
  iterator end() { return _operations_list.end(); }

  const_iterator begin() const { return _operations_list.cbegin(); }
  const_iterator end() const { return _operations_list.cend(); }

 private:
  std::string _process_name = {apiliteral::notprovided};

  std::string _bulk_operations = {apiliteral::notprovided};

  data_format_t _data_format = {data_format_t::gui};

  OperationsList _operations_list;
};

namespace debug {
namespace options {
void BulkOperations();
}
}  // namespace debug
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
namespace {
template<>
inline TraceStreamer& TraceStreamer::operator<<(const artdaq::database::configuration::BulkOperations& r)
{
  std::ostringstream s; s << r.to_string(); msg_append(s.str().c_str());
  return *this;
}
}
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATION_BULKOPERATIONS_H_ \
        */
