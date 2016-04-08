#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_OPERATIONS_COMMON_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_OPERATIONS_COMMON_H_

#include "artdaq-database/ConfigurationDB/common.h"

namespace artdaq {
namespace database {

namespace jsonutils {
class JSONDocument;
}

namespace configuration {
namespace options {

namespace literal {
constexpr auto operation_store = "store";
constexpr auto operation_load = "load";
constexpr auto operation_findconfigs = "findconfigs";
constexpr auto operation_buildfilter = "buildfilter";

constexpr auto operation_findversions = "findversions";
constexpr auto operation_findentities = "findentities";
constexpr auto operation_addconfig = "addconfig";
constexpr auto operation_newconfig = "newconfig";

constexpr auto type_default = "test_Default_Type001";
constexpr auto version_prefix = "test version";
constexpr auto global_configuration_prefix = "global config";
constexpr auto database_provider_mongo = "mongo";
constexpr auto database_provider_filesystem = "filesystem";
constexpr auto configuration_entity_any = "any";

constexpr auto operation = "operation";
constexpr auto version = "version";
constexpr auto configuration = "configuration";
constexpr auto dbprovider = "dbprovider";

constexpr auto dataformat = "dataformat";
constexpr auto filter = "filter";
constexpr auto collection = "collection";
constexpr auto configurable_entity = "configurable_entity";

constexpr auto notprovided = "notprovided";

constexpr auto operations = "operations";


constexpr auto default_filter = "{\"configurable_entity\":\"any\"}";

constexpr auto empty_search_result = "{\"search\":[ ]}";

constexpr auto colon = ":";
}

enum struct data_format_t { unknown, json, fhicl, gui, db };

using artdaq::database::jsonutils::JSONDocument;

class JsonSerializable {
 public:
  std::string const& operation() const noexcept;
  std::string const& operation(std::string const&);

  std::string const& globalConfigurationId() const noexcept;
  JSONDocument globalConfigurationId_jsndoc() const;
  std::string const& globalConfigurationId(std::string const&);

  std::string const& provider() const noexcept;
  std::string const& provider(std::string const&);

  data_format_t const& dataFormat() const noexcept;
  data_format_t const& dataFormat(data_format_t const&);
  data_format_t const& dataFormat(std::string const&);

  std::string const& searchFilter() const noexcept;
  virtual JSONDocument search_filter_jsondoc() const;

  virtual void read(std::string const&);
  virtual JSONDocument to_jsondoc() const;

  virtual std::string to_string() const;

 private:
  std::string _operation = {literal::operation_load};
  std::string _global_configuration_id = {literal::notprovided};
  std::string _provider = {literal::database_provider_filesystem};
  data_format_t _data_format = {data_format_t::unknown};

  std::string _search_filter = {literal::notprovided};
};

}  // namespace options

bool equal(std::string const&, std::string const&);
bool not_equal(std::string const&, std::string const&);
std::string quoted_(std::string const&);
std::string decode(options::data_format_t const&);

void trace_enable_CommonOperation();

}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_OPERATIONS_COMMON_H_ */
