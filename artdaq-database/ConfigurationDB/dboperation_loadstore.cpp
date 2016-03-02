#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_loadstore.h"
#include "artdaq-database/ConfigurationDB/dboperation_mongodb.h"
#include "artdaq-database/ConfigurationDB/dboperation_filedb.h"

#include "artdaq-database/FhiclJson/shared_literals.h"
#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/JsonDocument/JSONDocument_template.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

#include <boost/exception/diagnostic_information.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpLdStr_C"

bool equal( std::string const& left, std::string const& right ) {
    assert( !left.empty() );
    assert( !right.empty() );

    return left.compare( right ) == 0;
}

bool not_equal( std::string const& left, std::string const& right ) {
    return !equal( left, right );
}

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;
namespace cfol = cfo::literal;


using cfo::LoadStoreOperation;
using cfo::data_format_t;

std::string decode(data_format_t const& f) {
    switch(f) {
    default:
    case data_format_t::unknown :
        return "unknown";

    case data_format_t::fhicl :
        return "fhicl";

    case data_format_t::json :
        return "json";

    case data_format_t::gui :
        return "gui";
    }
}

std::string const& LoadStoreOperation::operation() const noexcept {
    assert( !_operation.empty() );

    return _operation;
}

std::string const& LoadStoreOperation::operation( std::string const& operation ) {
    assert( !operation.empty() );

    if ( not_equal( operation, cfol::operation_load )
            && not_equal( operation, cfol::operation_store ) ) {
        throw cet::exception( "Options" )
                << "Invalid operation; operation=" << operation << ".";
    }

    TRACE_( 11, "Options: Updating operation from "
            << _operation << " to " << operation << "." );

    _operation = operation;

    return _operation;
}

std::string const& LoadStoreOperation::type() const noexcept {
    assert( !_type.empty() );

    return _type;
}

std::string const& LoadStoreOperation::type( std::string const& type ) {
    assert( !type.empty() );

    if ( type.empty() ) {
        throw cet::exception( "Options" )
                << "Invalid type; type is empty.";
    }

    TRACE_( 12, "Options: Updating type from "
            << _type << " to " << type << "." );

    _type = type;

    return _type;
}

std::string const& LoadStoreOperation::version() const noexcept {
    assert( !_version.empty() );

    return _version;
}

std::string const& LoadStoreOperation::version( std::string const& version ) {
    assert( !version.empty() );

    if ( version.empty() ) {
        throw cet::exception( "Options" )
                << "Invalid version; version is empty.";
    }

    TRACE_( 13, "Options: Updating version from "
            << _version << " to " << version << "." );

    _version = version;

    return _version;

}

std::string const& LoadStoreOperation::globalConfigurationId() const noexcept {
    assert( !_global_configuration_id.empty() );

    return _global_configuration_id;
}

std::string const& LoadStoreOperation::globalConfigurationId( std::string const& global_configuration_id ) {
    assert( !global_configuration_id.empty() );

    if ( global_configuration_id.empty() ) {
        throw cet::exception( "Options" )
                << "Invalid global config id; version is empty.";
    }

    TRACE_( 14, "Options: Updating global_configuration_id from "
            << _global_configuration_id << " to " << global_configuration_id << "." );

    _global_configuration_id = global_configuration_id;

    return _global_configuration_id;
}

std::string const& LoadStoreOperation::provider() const noexcept {
    assert( !_provider.empty() );

    return _provider;
}

std::string const& LoadStoreOperation::provider( std::string const& provider ) {
    assert( !provider.empty() );

    if ( not_equal( provider, cfol::database_provider_filesystem )
            && not_equal( provider, cfol::database_provider_mongo ) ) {
        throw cet::exception( "Options" ) << "Invalid database provider; database provider=" << provider << ".";
    }

    TRACE_( 14, "Options: Updating provider from " << _provider << " to " << provider << "." );

    _provider = provider;

    return _provider;
}
data_format_t const& LoadStoreOperation::dataFormat() const noexcept
{
    assert( _data_format!= data_format_t::unknown);
    return _data_format;
}

data_format_t const& LoadStoreOperation::dataFormat(data_format_t const& data_format)
{
    assert( data_format!= data_format_t::unknown);

    TRACE_( 15, "Options: Updating data_format from "
            <<  decode(_data_format) << " to " <<  decode(data_format) << "." );

    _data_format = data_format;

    return _data_format;
}

data_format_t const& LoadStoreOperation::dataFormat(std::string const& format)
{
    assert(!format.empty());

    TRACE_( 16, "Options: dataFormat args format=<" <<  format << ">." );

    if(format.compare("fhicl")==0) {
        dataFormat(data_format_t::fhicl);
    } else if (format.compare("json")==0) {
        dataFormat(data_format_t::json);
    } else if (format.compare("gui")==0) {
        dataFormat(data_format_t::gui);
    } else {
        dataFormat(data_format_t::unknown);
    }

    return _data_format;
}


using Options = cfo::LoadStoreOperation;
using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::FhiclData;
using artdaq::database::jsonutils::JSONDocument;
using artdaq::database::jsonutils::JSONDocumentBuilder;


namespace detail {
typedef JsonData ( *provider_load_t ) ( Options const& /*options*/, JsonData const& /*search_payload*/);
typedef void ( *provider_store_t ) ( Options const& /*options*/, JsonData const& /*insert_payload*/ );

void store_configuration( Options const& options, std::string& conf ) {
    assert( !conf.empty() );
    assert( options.operation().compare( cfo::literal::operation_store ) == 0 );

    TRACE_( 15, "store_configuration: begin" );

    if ( conf.empty() ) {
        TRACE_( 15, "Error in store_configuration: Invalid configuration; configuration is empty." );

        throw cet::exception( "store_configuration" )
                << "Invalid configuration; configuration is empty.";
    }

    if ( not_equal( options.provider(), cfol::database_provider_filesystem )
            && not_equal( options.provider(), cfol::database_provider_mongo ) ) {
        TRACE_( 15, "Error in store_configuration: Invalid database provider; database provider="
                << options.provider() << "." );

        throw cet::exception( "store_configuration" )
                << "Invalid database provider; database provider="
                << options.provider() << ".";
    }

    auto data = JsonData {conf};
    auto returnValue = std::string {};
    auto returnValueChanged = bool {false};

    switch(options.dataFormat())
    {
    default:
    case data_format_t::json:
        //do nothing
        break;

    case data_format_t::unknown:
        throw cet::exception( "store_configuration" ) << "Invalid data format; data format="
                <<  decode(options.dataFormat()) << ".";
        break;

    case data_format_t::fhicl:
        //convert from fhicl to json and back to fhicl
        data = FhiclData { conf };

        auto fhicl = FhiclData {};

        if ( !data.convert_to<FhiclData>( fhicl ) ) {
            TRACE_( 17, "store_configuration: Unable to convert json data to fcl; json=<" << data.json_buffer << ">" );

            throw cet::exception( "store_configuration" )
                    << "Unable to reverse fhicl-to-json convertion";
        } else {
            TRACE_( 17, "store_configuration: Converted json data to fcl; json=<" << data.json_buffer << ">" );
            TRACE_( 17, "store_configuration: Converted json data to fcl; fcl=<" << fhicl.fhicl_buffer << ">" );
        }

        returnValueChanged=true;

        break;

    }

    TRACE_( 15, "store_configuration: json_buffer=<" << data.json_buffer << ">" );


    //create a json document to be inserted into the database
    auto builder = JSONDocumentBuilder {};
    builder.createFromData( data.json_buffer );
    builder.addToGlobalConfig( options.globalConfigurationId_jsndoc() );
    builder.setVersion( options.version_jsndoc() );
    auto document = std::move( builder.extract() );
    auto insert_payload = JsonData {"{\"document\":" + document.to_string()
                                    + ", \"collection\":\"" + options.type() + "\"}"
                                   };

    TRACE_( 15, "store_configuration: insert_payload=<" << insert_payload.json_buffer << ">" );

    auto dispatch_persistence_provider = []( std::string const & name ) {
        auto providers = std::map<std::string, provider_store_t> {
            {cfol::database_provider_mongo,      cf::mongo::store},
            {cfol::database_provider_filesystem, cf::filesystem::store}
        };

        return providers.at( name );
    };

    dispatch_persistence_provider(options.provider())(options,insert_payload);

    if(returnValueChanged)
        conf.swap( returnValue );

    TRACE_( 15, "store_configuration: end" );
}

void load_configuration( Options const& options, std::string& conf ) {
    assert( conf.empty() );
    assert( options.operation().compare( cfo::literal::operation_load ) == 0 );

    TRACE_( 16, "load_configuration: begin" );

    if ( !conf.empty() ) {
        throw cet::exception( "load_configuration" )
                << "Invalid configuration; configuration is not empty.";
    }

    if ( not_equal( options.provider(), cfol::database_provider_filesystem )
            && not_equal( options.provider(), cfol::database_provider_mongo ) ) {
        TRACE_( 16, "Error in load_configuration: Invalid database provider; database provider=" << options.provider() << "." );

        throw cet::exception( "load_configuration" )
                << "Invalid database provider; database provider="
                << options.provider() << ".";
    }

    auto search_payload = JsonData {"{\"filter\":" + options.search_filter_jsondoc().to_string()+
                                    + ", \"collection\":\"" + options.type() + "\"}"
                                   };

    TRACE_( 16, "load_configuration: search_payload=<" << search_payload.json_buffer << ">" );

    auto dispatch_persistence_provider = []( std::string const & name ) {
        auto providers = std::map<std::string, provider_load_t> {
            {cfol::database_provider_mongo,      cf::mongo::load},
            {cfol::database_provider_filesystem, cf::filesystem::load}
        };

        return providers.at( name );
    };

    auto search_result =  dispatch_persistence_provider(options.provider() )(options,search_payload);


    auto returnValue = std::string {};
    auto returnValueChanged = bool {false};

    switch(options.dataFormat())
    {
    default:
    case data_format_t::json: {
        auto document = JSONDocument {search_result.json_buffer};
        returnValue=document.findChild(db::jsonutils::literal::document).to_string();

        returnValueChanged=true;
        break;
    }
    case data_format_t::unknown:
        throw cet::exception( "load_configuration" )
                << "Invalid data format; data format="
                <<  decode(options.dataFormat()) << ".";
        break;

    case data_format_t::fhicl: {
        auto document = JSONDocument {search_result.json_buffer};
        auto json = JsonData {document.findChild(db::jsonutils::literal::document).to_string()};

        auto fhicl = FhiclData {};
        if ( !json.convert_to<FhiclData>( fhicl ) ) {
            TRACE_( 16, "load_configuration: Unable to convert json data to fcl; json=<" << json.json_buffer << ">" );

            throw cet::exception( "load_configuration" )
                    << "Unable to reverse fhicl-to-json convertion";
        }
	returnValue = fhicl.fhicl_buffer;
	
        returnValueChanged=true;

        break;
    }
    }

    if(returnValueChanged)
        conf.swap( returnValue );

    TRACE_( 15, "load_configuration: end" );
}
}


cf::result_pair_t cf::store_configuration ( Options const& options, std::string& conf) noexcept
{
    try {
        detail::store_configuration (options,conf);
        return cf::result_pair_t {true,"Success"};
    } catch(...) {
        return cf::result_pair_t {false,boost::current_exception_diagnostic_information()};
    }
}

cf::result_pair_t cf::load_configuration ( Options const& options, std::string& conf) noexcept
{
    try {
        detail::load_configuration (options,conf);
        return cf::result_pair_t {true,"Success"};
    } catch(...) {
        return cf::result_pair_t {false,boost::current_exception_diagnostic_information()};
    }
}

namespace jul = artdaq::database::jsonutils::literal;
using namespace artdaq::database::jsonutils;

JSONDocument LoadStoreOperation::version_jsndoc() const {
    auto kvp = std::make_pair<std::string, std::string>(
                   jul::version, _version.c_str() );
    return toJSONDocument( kvp );
}

JSONDocument LoadStoreOperation::globalConfigurationId_jsndoc() const {
    auto kvp = std::make_pair<std::string, std::string>(
                   jul::configurations, _global_configuration_id.c_str() );

    return toJSONDocument( kvp );
}

constexpr auto colon = ":";

std::string  quoted_( std::string const& text ) {
    return "\"" + text + "\"";
}

JSONDocument LoadStoreOperation::search_filter_jsondoc() const {
    std::stringstream ss;
    ss << "{";
    ss << quoted_( jul::configurations_name ) << colon << quoted_( _global_configuration_id );
    //ss<<"," << quoted_( jul::version) << colon << quoted_(_version);
    ss << "}";

    return {ss.str()};
}

void  cf::trace_enable_LoadStoreOperation() {
    TRACE_CNTL( "name",    TRACE_NAME );
    TRACE_CNTL( "lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL );
    TRACE_CNTL( "modeM", 1LL );
    TRACE_CNTL( "modeS", 1LL );

    TRACE_( 0, "artdaq::database::configuration::LoadStoreOperation" << "trace_enable" );
}
