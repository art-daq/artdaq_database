#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_H_

#include "artdaq-database/ConfigurationDB/dboperations_common.h"

namespace artdaq {
namespace database {
namespace jsonutils {
class JSONDocument;
}

namespace configuration {
namespace options {

using artdaq::database::jsonutils::JSONDocument;

class LoadStoreOperation final : public JsonSerializable {
 public:
  std::string const& type() const noexcept;
  std::string const& type(std::string const&);

  std::string const& version() const noexcept;
  JSONDocument version_jsndoc() const;
  JSONDocument search_filter_jsondoc() const override final;

  std::string const& version(std::string const&);
  std::string const& configurableEntity() const noexcept;
  std::string const& configurableEntity(std::string const&);
  JSONDocument configurableEntity_jsndoc() const;
  void read(std::string const&) override final;
  JSONDocument to_jsondoc() const override final;

 private:
  std::string _type = {literal::notprovided};
  std::string _version = {literal::notprovided};
  std::string _configurable_entity = {literal::notprovided};
};

}  // namespace options

using Options = options::LoadStoreOperation;

// both functions return converted conf file
result_pair_t store_configuration(Options const& /*options*/, std::string& /*conf*/) noexcept;
result_pair_t load_configuration(Options const& /*options*/, std::string& /*conf*/) noexcept;

namespace guiexports {
result_pair_t store_configuration(std::string const& /*search_filter*/, std::string const& /*conf*/) noexcept;
result_pair_t load_configuration(std::string const& /*search_filter*/, std::string& /*conf*/) noexcept;
}
void trace_enable_LoadStoreOperation();
void trace_enable_LoadStoreOperationDetail();
void trace_enable_DBOperationMongo();
void trace_enable_DBOperationFileSystem();
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_H_ */
