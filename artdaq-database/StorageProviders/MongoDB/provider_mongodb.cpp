#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"
#include "artdaq-database/StorageProviders/common.h"

#include <bsoncxx/builder/basic/helpers.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/find.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:MongoDB_C"

namespace bbs = bsoncxx::builder::stream;

namespace artdaq {
namespace database {

namespace detail {
template <typename T>
constexpr std::uint8_t static_cast_as_uint8_t(T const& t) {
  return static_cast<std::uint8_t>(t);
}

}  // namespace detail

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

using artdaq::database::basictypes::JsonData;
using artdaq::database::mongo::MongoDB;

std::string dequote(std::string);

template <>
template <>
std::vector<JsonData> StorageProvider<JsonData, MongoDB>::load(JsonData const& search) {
  TRACE_(3, "StorageProvider::MongoDB::load() begin");
  TRACE_(3, "StorageProvider::MongoDB::load() args search=<" << search.json_buffer << ">");

  auto returnCollection = std::vector<JsonData>();

  auto bson_document = bsoncxx::from_json(search.json_buffer);

  if (!bson_document) throw cet::exception("MongoDB") << "Invalid Search criteria; json_buffer" << search.json_buffer;

  auto extract_value = [&bson_document](auto const& name) {
    auto view = bson_document->view();
    auto element = view.find(name);

    if (element == view.end())
      throw cet::exception("MongoDB") << "Search JsonData is missing the \"" << name << "\" element.";

    return element->get_value();
  };

  auto collection_name = dequote(bsoncxx::to_json(extract_value("collection")));

  auto collection = _provider->connection().collection(collection_name);

  auto filter = bsoncxx::builder::core(false);

  try {
    auto tmp = extract_value("filter");

    TRACE_(3, "StorageProvider::MongoDB::load() user provided filter=<" << bsoncxx::to_json(tmp) << ">");

    using detail::static_cast_as_uint8_t;
    using value_type_t = bsoncxx::type;
    using namespace bsoncxx::builder::stream;

    switch (static_cast_as_uint8_t(tmp.type())) {
      default: {
        TRACE_(3, "StorageProvider::MongoDB::load() case  default=<" << bsoncxx::to_json(tmp) << ">");

        filter.append(tmp);
        break;
      }
      case static_cast_as_uint8_t(value_type_t::k_document): {
        auto doc = bbs::document{};
        doc << concatenate{tmp.get_document().value};
        filter.concatenate(doc);
        break;
      }
      case static_cast_as_uint8_t(value_type_t::k_oid): {
        auto doc = bbs::document{};
        doc << "_id" << open_document << "$in" << open_array << tmp << close_array << close_document;
        filter.concatenate(doc);
        break;
      }
    }
    TRACE_(2, "search filter=<" << bsoncxx::to_json(filter.view_document()) << ">");
  } catch (cet::exception const&) {
    TRACE_(2, "search filter is missing");
  }

  auto size = collection.count(filter.view_document());
  returnCollection.reserve(size);

  auto cursor = collection.find(filter.view_document());

  TRACE_(3, "found_count=" << size);

  for (auto& doc : cursor) returnCollection.emplace_back(bsoncxx::to_json(doc));

  return returnCollection;
}

template <>
object_id_t StorageProvider<JsonData, MongoDB>::store(JsonData const& data) {
  TRACE_(4, "StorageProvider::MongoDB::store() begin");

  auto bson_document = bsoncxx::from_json(data.json_buffer);

  if (!bson_document) throw cet::exception("MongoDB") << "Invalid data; json_buffer" << data.json_buffer;

  auto extract_value = [&bson_document](auto const& name) {
    auto view = bson_document->view();
    auto element = view.find(name);

    if (element == view.end())
      throw cet::exception("MongoDB") << "Search JsonData is missing the \"" << name << "\" element.";

    return element->get_value();
  };

  auto collection_name = dequote(bsoncxx::to_json(extract_value("collection")));
  auto collection = _provider->connection().collection(collection_name);

  auto value = extract_value("document");

  auto filter = bsoncxx::builder::core(false);

  auto isNew = bool{true};
  auto replaceOne = bool{false};
  try {
    auto tmp = extract_value("filter");

    TRACE_(3, "StorageProvider::MongoDB::load() user provided filter=<" << bsoncxx::to_json(tmp) << ">");

    using detail::static_cast_as_uint8_t;
    using value_type_t = bsoncxx::type;
    using namespace bsoncxx::builder::stream;

    switch (static_cast_as_uint8_t(tmp.type())) {
      default: {
        TRACE_(3, "StorageProvider::MongoDB::load() case  default=<" << bsoncxx::to_json(tmp) << ">");

        filter.append(tmp);
        break;
      }
      case static_cast_as_uint8_t(value_type_t::k_document): {
        auto doc = bbs::document{};
        doc << concatenate{tmp.get_document().value};
        filter.concatenate(doc);
        break;
      }
      case static_cast_as_uint8_t(value_type_t::k_oid): {
        auto doc = bbs::document{};
        doc << "_id" << open_document << "$in" << open_array << tmp << close_array << close_document;
        filter.concatenate(doc);
        replaceOne = true;
        break;
      }
    }
    TRACE_(2, "search filter=<" << bsoncxx::to_json(filter.view_document()) << ">");
    isNew = false;
  } catch (cet::exception const&) {
    TRACE_(4, "Search filter is missing, proceeding with insert.");
  }

  if (isNew) {
    auto result = collection.insert_one(value.get_document().value);

    auto object_id = object_id_t(bsoncxx::to_json(result->inserted_id()));

    TRACE_(5, "inserted_id=" << object_id);

    return object_id;
  } else if (replaceOne) {
    auto result[[gnu::unused]] = collection.replace_one(filter.view_document(), value.get_document().value);

    TRACE_(8, "modified_count=" << result->modified_count());
    return "";
  } else {
    auto operation = std::string("$set");
    auto path = extract_value("path");

    auto update = bbs::document{};
    update << "$set" << open_document << "path" << value << close_document;

    TRACE_(2, "update data=<" << bsoncxx::to_json(update) << ">");
    TRACE_(2, "update filter=<" << bsoncxx::to_json(filter.view_document()) << ">");

    auto result[[gnu::unused]] = collection.update_many(filter.view_document(), update);

    TRACE_(8, "modified_count=" << result->modified_count());

    return "";
  }
}

template <>
template <>
std::vector<JsonData> StorageProvider<JsonData, MongoDB>::findGlobalConfigs(JsonData const& search) {
  assert(!search.json_buffer.empty());
  auto returnCollection = std::vector<JsonData>();

  TRACE_(4, "StorageProvider::MongoDB::findGlobalConfigs() begin");
  TRACE_(4, "StorageProvider::MongoDB::findGlobalConfigs() args data=<" << search.json_buffer << ">");

  auto filter = bsoncxx::builder::core(false);
  auto search_filter_doc = bbs::document{};
  // FIXME create search filter
  filter.concatenate(search_filter_doc);

  auto collectionNames = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionName[[gnu::unused]] : collectionNames) {
    /*
     auto collection = _provider->connection().collection(collectionName);

    auto collection_filter= bsoncxx::builder::core(false);
    auto collection_filter_doc = bbs::document {};
    //FIXME create search filter
    collection_filter.concatenate(collection_filter_doc);

    auto cursor = collection.find(collection_filter.view_document());

    for (auto & doc : cursor) {
      returnCollection.emplace_back(bsoncxx::to_json(*doc));
    }

    */
  }

  return returnCollection;
}

template <>
template <>
std::vector<JsonData> StorageProvider<JsonData, MongoDB>::buildConfigSearchFilter(JsonData const& search) {
  assert(!search.json_buffer.empty());
  auto returnCollection = std::vector<JsonData>();

  TRACE_(4, "StorageProvider::MongoDB::buildConfigSearchFilter() begin");
  TRACE_(4, "StorageProvider::MongoDB::buildConfigSearchFilter() args data=<" << search.json_buffer << ">");

  return returnCollection;
}

std::string dequote(std::string s) {
  if (s[0] == '"' && s[s.length() - 1] == '"')
    return s.substr(1, s.length() - 2);
  else
    return s;
}

namespace mongo {
void trace_enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::mongo::"
                << "trace_enable");
}
}  // namespace mongo
}  // namespace database
}  // namespace artdaq
