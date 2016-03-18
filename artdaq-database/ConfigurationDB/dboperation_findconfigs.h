#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_FINDCONFIGS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_FINDCONFIGS_H_

#include "artdaq-database/ConfigurationDB/dboperations_common.h"

namespace artdaq {
namespace database {
namespace jsonutils {
class JSONDocument;
}

namespace configuration {
namespace options {

using artdaq::database::jsonutils::JSONDocument;

class FindConfigsOperation final : public JsonSerializable {
 public:
  void read(std::string const&) override final;
  JSONDocument search_filter_jsondoc() const override final;

  std::string const& configurableEntity() const noexcept;
  std::string const& configurableEntity(std::string const&);
  JSONDocument configurableEntity_jsndoc() const;

 private:
  std::string _search_filter = {literal::notprovided};
  std::string _configurable_entity = {literal::notprovided};
};

}  // namespace options

// both functions return converted conf file
result_pair_t find_global_configurations(options::FindConfigsOperation const& /*options*/) noexcept;
result_pair_t build_global_configuration_search_filter(options::FindConfigsOperation const& /*options*/) noexcept;

namespace guiexports {
result_pair_t find_global_configurations(std::string const& /*search_filter*/) noexcept;
result_pair_t build_global_configuration_search_filter(std::string const& /*search_filter*/) noexcept;
}
void trace_enable_FindConfigsOperation();
void trace_enable_FindConfigsOperationDetail();
void trace_enable_DBOperationMongo();
void trace_enable_DBOperationFileSystem();
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_FINDCONFIGS_H_ */
