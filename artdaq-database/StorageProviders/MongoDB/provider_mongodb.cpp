#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"
#include "artdaq-database/BasicTypes/basictypes.h"


#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#ifdef TRACE_NAME
  #undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:MongoDB_C"

namespace bbs = bsoncxx::builder::stream;

namespace artdaq{
namespace database{


using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

using artdaq::database::basictypes::JsonData;
using artdaq::database::mongo::MongoDB;

std::string dequote(std::string);

template<>
template <>
std::vector<JsonData> StorageProvider<JsonData, MongoDB>::load(JsonData const& search)
{
    auto returnCollection = std::vector<JsonData>();

    auto bson_document = bsoncxx::from_json(search.json_buffer);

    if (!bson_document)
        throw cet::exception("MongoDB") << "Invalid Search criteria; json_buffer" << search.json_buffer;

    auto nested_document = [&bson_document](auto const & name) {
        auto view = bson_document->view();
        auto element = view.find(name);

        if (element == view.end())
            throw cet::exception("MongoDB") << "Search criteria is missing the \"" << name << "\" element.";

        return element->get_value();
    };

    auto collection_name = dequote(bsoncxx::to_json(nested_document("collection")));

    auto collection = _provider->connection().collection(collection_name);

    auto filter = bsoncxx::types::b_document();

    try {
        filter = nested_document("filter").get_document();
        TRACE_( 2, "search filter=" <<  bsoncxx::to_json(filter) );
    } catch (cet::exception const&) {
	TRACE_( 2, "search filter is missing");
    }

    auto size = collection.count(filter);
    returnCollection.reserve(size);
    
    auto cursor = collection.find(filter);
    
    TRACE_( 3,  "found_count=" << size );

    for (auto & doc : cursor)
        returnCollection.emplace_back(bsoncxx::to_json(doc));
    
    return returnCollection;
}


template<>
void StorageProvider<JsonData, MongoDB>::store(JsonData const& data)
{
    auto bson_document = bsoncxx::from_json(data.json_buffer);

    if (!bson_document)
        throw cet::exception("MongoDB") << "Invalid data; json_buffer" << data.json_buffer;

    auto nested_document = [&bson_document](auto const & name) {
        auto view = bson_document->view();
        auto element = view.find(name);

        if (element == view.end())
            throw cet::exception("MongoDB") << "Search criteria is missing the \"" << name << "\" element.";

        return element->get_value();
    };

    auto collection_name = dequote(bsoncxx::to_json(nested_document("collection")));

    auto collection = _provider->connection().collection(collection_name);

    auto document = nested_document("document");

    auto filter = bbs::document {};

    auto isNew = bool {true};

    try {
        auto document_id = nested_document(collection_name + "_id");
        filter << (collection_name + "_id") << document_id;
	TRACE_( 4,   "search filter=" <<  bsoncxx::to_json(filter));

        isNew = false;
    } catch (cet::exception const&) {
	TRACE_( 4, "search filter is missing");
    }

    if (isNew) {
        auto insert = bbs::document {}
                      << (collection_name + "_id") << 0
                      << "document" << document << finalize;

        auto result [[gnu::unused]] =  collection.insert_one(insert);

        TRACE_(5, "inserted_id=" <<  bsoncxx::to_json(result->inserted_id()));

    } else {
        auto update = bbs::document {};

        update <<  "$set" << open_document << "document" << document << close_document
               <<  "$currentDate" <<  open_document << "updated"
               <<  open_document << "$type" << "date" << close_document
               <<  close_document;

        auto result [[gnu::unused]] = collection.update_many(filter, update);

        TRACE_( 6,"modified_count=" << result->modified_count());
    }
}


std::string dequote(std::string s)
{
    if (s[0] == '"' && s[s.length() - 1] == '"')
        return s.substr(1, s.length() - 2);
    else
        return s;
}
namespace mongo{
  void  trace_enable()
  {
    TRACE_CNTL( "name",    TRACE_NAME);
    TRACE_CNTL( "lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL ); 
    TRACE_CNTL( "modeM", 1LL );
    TRACE_CNTL( "modeS", 1LL );

    
    TRACE_( 0, "artdaq::database::mongo::" << "trace_enable");
  }
} // namespace mongo
} //namespace database
} //namespace artdaq
