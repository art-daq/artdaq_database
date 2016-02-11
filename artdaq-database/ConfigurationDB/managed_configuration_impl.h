#include "artdaq-database/ConfigurationDB/managed_configuration.h"

using artdaq::database::configuration::ManagedConfiguration;
namespace regex
{
constexpr auto parse_aliases = "[\\s\\S]*\"aliases\"\\s*:\\s*\"(\\S*?)\"";
}

template <typename CONF, typename PROVIDER>
template <typename FILTER>
std::vector<typename ManagedConfiguration<CONF, PROVIDER>::SelfType>
ManagedConfiguration<CONF, PROVIDER>::load(FILTER& f)
{
    auto results = std::vector<typename ManagedConfiguration<CONF, PROVIDER>::SelfType>();

    auto collection = provider()->load(f);

    if (collection.size() < 1)
        return results;

    results.reserve(collection.size());

    for (auto && element : collection) {
        std::stringstream is;
        is.str(element.json_buffer);
        auto conf = typename ManagedConfiguration<CONF, PROVIDER>::PersistType(CONF());
        conf.read(is);
        results.emplace_back(conf.get(), _provider);
    }

    return results;
}

template <typename CONF, typename PROVIDER>
typename ManagedConfiguration<CONF, PROVIDER>::SelfType&
ManagedConfiguration<CONF, PROVIDER>::store()
{
    std::stringstream os;
    self().write(os);
    provider()->store(StorableType(os.str()));
    return self();
}

template <typename CONF, typename PROVIDER>
typename ManagedConfiguration<CONF, PROVIDER>::SelfType&
ManagedConfiguration<CONF, PROVIDER>::assignAlias(std::string const& alias)
{    
    auto versioned_typename=std::string(self().versioned_typename());
    
    std::stringstream os;
    
    os << "{\"document\":";
    os <<               "{\"aliases\":\"" << alias << "\"},";
    os <<  "\"update\"  :{\"path\":\"document.aliases\",\"update:\":\"$push\"},";
    os <<  "\"collection\":\"" << versioned_typename << "\", \"" << versioned_typename << "_id\":\"" << self().metadata().version_release << "\"";
    os << "}";
    
    provider()->store(StorableType(os.str()));
    return self();    
}

template <typename CONF, typename PROVIDER>
typename ManagedConfiguration<CONF, PROVIDER>::SelfType&
ManagedConfiguration<CONF, PROVIDER>::addToGlobalConfiguration(std::string const&)
{
    return self();
}

template <typename CONF, typename PROVIDER>
typename ManagedConfiguration<CONF, PROVIDER>::SelfType&
ManagedConfiguration<CONF, PROVIDER>::markDeleted()
{
    auto versioned_typename=std::string(self().versioned_typename());
    
    std::stringstream os;
    
    os << "{\"document\":";
    os <<               "{\"isdeleted\":true},";
    os <<  "\"update\"  :{\"path\":\"document.status\",\"update:\":\"$set\"},";
    os <<  "\"collection\":\"" << versioned_typename << "\", \"" << versioned_typename << "_id\":\"" << self().metadata().version_release << "\"";
    os << "}";
    
    provider()->store(StorableType(os.str()));
    return self();    
}

template <typename CONF, typename PROVIDER>
std::vector<std::string>
ManagedConfiguration<CONF, PROVIDER>::listAliases()
{
    auto results = std::vector<std::string>();

	
    auto versioned_typename=std::string(self().versioned_typename());
    
    std::stringstream os;
    
    os << "{\"filter\":";
    os <<                "{\"" << versioned_typename << "_id\":\"" << self().metadata().version_release << "\",";
    os <<                      "\"document.status.isdeleted\":false" ;
    os <<                "}";
    os <<  "\"collection\":\"" << versioned_typename << "\"";
    os << "}";
    

    auto collection = provider()->load(os.str());

    auto collection_size= collection.size();
    
    if (collection_size < 1)
        return results;

    if(collection_size > 1)
    {
       TRACE_( 2, "too many results found; count" << collection_size );
    }
    
    auto document= collection.at(0);
    auto ex = std::regex( {regex::parse_aliases});
    auto aliases = std::smatch();

    if (!std::regex_search(document.json_buffer, aliases, ex)) {
        TRACE_( 3, "document has no aliases, regex_search()==false; JSON buffer: " + document.json_buffer);
        return results;
    }	
        std::stringstream is;
        
	is.str(element.json_buffer);
        
	auto conf = typename ManagedConfiguration<CONF, PROVIDER>::PersistType(CONF());
        conf.read(is);
	
        results.emplace_back();
    }
    results.reserve(collection.size());

    return results;

    provider()->store(StorableType(os.str()));
    return self(); 
}

template <typename CONF, typename PROVIDER>
std::vector<std::tuple<std::string, std::time_t>>
        ManagedConfiguration<CONF, PROVIDER>::listAliasHistory()
{
    return {};
}

template <typename CONF, typename PROVIDER>
std::vector<std::string>
ManagedConfiguration<CONF, PROVIDER>::listGlobalConfigurations()
{
    return {};
}

