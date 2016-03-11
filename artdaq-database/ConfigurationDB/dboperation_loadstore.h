#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_H_

#include "artdaq-database/ConfigurationDB/common.h"

namespace artdaq{
namespace database{
  
namespace jsonutils{
class JSONDocument;
}

namespace configuration{
namespace options{


namespace literal{
constexpr auto operation_store = "store";
constexpr auto operation_load = "load";
constexpr auto type_default = "test_Default_Type001";
constexpr auto version_prefix = "test version";
constexpr auto global_configuration_prefix = "global config";
constexpr auto database_provider_mongo = "mongo";
constexpr auto database_provider_filesystem = "filesystem";
constexpr auto configuration_entity_any="any";

constexpr auto operation="operation";
constexpr auto version="version";
constexpr auto configuration="configuration";
constexpr auto dbprovider="dbprovider";

constexpr auto dataformat="dataformat";
constexpr auto filter="filter";
constexpr auto collection="collection";
constexpr auto configurable_entity="configurable_entity";

constexpr auto notprovided="notprovided";

constexpr auto default_filter="{\"configurable_entity\":\"any\"}";
}

using artdaq::database::jsonutils::JSONDocument;

enum struct data_format_t{unknown, json, fhicl, gui,db};

class LoadStoreOperation final: public JsonSerializable
{
public:
    std::string const& operation() const noexcept;
    std::string const& operation ( std::string const& );

    std::string const& type() const noexcept;
    std::string const& type ( std::string const& );

    std::string const& version() const noexcept;
    JSONDocument version_jsndoc() const;
    JSONDocument search_filter_jsondoc() const;

    std::string const& version ( std::string const& );

    std::string const& globalConfigurationId() const noexcept;
    JSONDocument globalConfigurationId_jsndoc() const;

    std::string const& globalConfigurationId ( std::string const& );

    std::string const& provider() const noexcept;
    std::string const& provider ( std::string const& );

    std::string const& configurableEntity() const noexcept;
    std::string const& configurableEntity ( std::string const& );
    JSONDocument configurableEntity_jsndoc() const;

    void read(std::string const& );
    
    data_format_t const& dataFormat() const noexcept;
    data_format_t const& dataFormat (data_format_t const& );
    data_format_t const& dataFormat (std::string const& );
    
private:
    std::string _operation= {literal::operation_load};
    std::string _type= {literal::type_default};
    std::string _version= {literal::notprovided};
    std::string _global_configuration_id = {literal::notprovided};
    std::string _provider = {literal::database_provider_filesystem};
    data_format_t _data_format ={data_format_t::unknown};
    std::string _configurable_entity = {literal::notprovided};

    std::string _search_filter= {literal::notprovided};
    
};

} //namespace options

using Options= options::LoadStoreOperation;

//both functions return converted conf file
result_pair_t store_configuration ( Options const& /*options*/, std::string& /*conf*/ ) noexcept;
result_pair_t load_configuration ( Options const& /*options*/, std::string& /*conf*/ ) noexcept;

namespace guiexports {
result_pair_t store_configuration ( std::string const& /*search_filter*/, std::string const& /*conf*/ ) noexcept;
result_pair_t load_configuration ( std::string const& /*search_filter*/, std::string& /*conf*/ ) noexcept;
}
void  trace_enable_LoadStoreOperation();
void  trace_enable_LoadStoreOperationMongo();
void  trace_enable_LoadStoreOperationFileSystem();
} //namespace configuration
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_H_ */
