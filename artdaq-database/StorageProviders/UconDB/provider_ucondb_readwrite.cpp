#include "artdaq-database/StorageProviders/UconDB/provider_ucondb_headers.h"

#include "artdaq-database/BasicTypes/data_json.h"
#include "artdaq-database/DataFormats/Json/json_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "provider_ucondb_readwrite.cpp"

namespace artdaq {
namespace database {

using artdaq::database::json::array_t;
using artdaq::database::json::JsonReader;
using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::sharedtypes::unwrap;

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, UconDB>::readDocument(JSONDocument const& arg) {
  TLOG(13) << "UconDB::readDocument() begin";
  TLOG(13) << "UconDB::readDocument() args=<" << arg << ">";

  auto returnCollection = std::vector<JSONDocument>();

  auto filter_document = JSONDocument{};

  try {
    filter_document = arg.findChildDocument(jsonliteral::filter);
  } catch (...) {
    TLOG(13) << "UconDB::readDocument() No filter was found.";
  }

  auto collection_name = std::string{};

  try {
    collection_name = filter_document.findChild(jsonliteral::collection).value();
  } catch (...) {
    TLOG(13) << "UconDB::readDocument() Filter must have the collection element.";
  }

  if (collection_name.empty()) {
    collection_name = arg.findChild(jsonliteral::collection).value();
  }

  confirm(!collection_name.empty());

  TLOG(13) << "UconDB::readDocument() collection_name=<" << collection_name << ">";

  auto fl = folders(_provider);

  bool found = (std::find(fl.begin(), fl.end(), to_lower(collection_name)) != fl.end());

  if (!found) {
    throw runtime_error("UconDB") << "UconDB Missing collection; collection_name=" << collection_name;
  }

  auto oids = std::list<object_id_t>{};

  try {
    auto id_json = filter_document.findChild(jsonliteral::id).to_string();

    auto idAST = object_t{};

    if (!JsonReader().read(id_json, idAST)) {
      throw runtime_error("UconDB") << "UconDB::readDocument() Unabe to read JSON, json=<" << id_json << ">";
    }

    auto& id = unwrap(idAST).value_as<object_t>(jsonliteral::id);

    try {
      oids.push_back(unwrap(id).value_as<std::string>(jsonliteral::oid));
    } catch (...) {
    }

    try {
      auto& oidvals = unwrap(id).value_as<array_t>(jsonliteral::in);
      for (auto& oidval : oidvals) {
        oids.push_back(unwrap(oidval).value_as<std::string>(jsonliteral::oid));
      }
    } catch (...) {
    }

  } catch (...) {
    auto filter_json = filter_document.to_string();
    TLOG(13) << "UconDB::readDocument() invalid filter_json=<" << filter_json << ">.";
  }

  for (auto const& oid : oids) {
    auto result = get_object(_provider, collection_name, oid);

    if (!result.first) {
      throw runtime_error("UconDB") << "UconDB search failed, error=" << result.second;
    }

    returnCollection.emplace_back(result.second);
  }

  return returnCollection;
}

template <>
object_id_t StorageProvider<JSONDocument, UconDB>::writeDocument(JSONDocument const& arg) {
  TLOG(14) << "UconDB::writeDocument() begin";
  TLOG(14) << "UconDB::writeDocument() args=<" << arg << ">";

  auto user_document = arg.findChildDocument(jsonliteral::document);

  auto filter_document = JSONDocument{};

  try {
    filter_document = arg.findChildDocument(jsonliteral::filter);
  } catch (...) {
    TLOG(14) << "UconDB::writeDocument() No filter was found.";
  }

  auto collection_name = std::string{};

  try {
    collection_name = user_document.findChild(jsonliteral::collection).value();
  } catch (...) {
    TLOG(14) << "UconDB::writeDocument() User document must have the collection element.";
  }

  if (collection_name.empty()) {
    try {
      collection_name = filter_document.findChild(jsonliteral::collection).value();
    } catch (...) {
      TLOG(14) << "UconDB::writeDocument() Filter should have the collection element.";
    }
  }

  if (collection_name.empty()) {
    collection_name = arg.findChild(jsonliteral::collection).value();
  }

  confirm(!collection_name.empty());

  TLOG(14) << "UconDB::writeDocument() collection_name=<" << collection_name << ">";

  auto result = create_folder(_provider, collection_name);

  if (!result.first) {
    throw runtime_error("UconDB") << "UconDB failed creating a new folder; folder name=" << collection_name << ", error=" << result.second;
  }

  auto oid = object_id_t{ouid_invalid};

#if 0  
  //auto isNew = bool{true};

  try {
    auto oid_json = filter_document.findChild(jsonliteral::id).value();
    TLOG(14) << "UconDB::writeDocument() Found filter=<" << oid_json << ">";
    oid = extract_oid(oid_json);
  //  isNew = false;
    TLOG(14) << "UconDB::writeDocument() Using provided oid=<" << oid << ">";
  } catch (...) {
  }
#endif

  if (oid == object_id_t{ouid_invalid}) {
    oid = generate_oid();
    TLOG(14) << "UconDB::writeDocument() Using generated oid=<" << oid << ">";
  }

  auto id = to_id(oid);

  JSONDocumentBuilder builder(user_document);
  builder.setObjectID({id});
  builder.setCollection({to_json(jsonliteral::collection, collection_name)});
  builder.markReadonly();

  auto json = builder.to_string();

  TLOG(14) << "UconDB::writeDocument() json=<" << json << ">.";

  auto tags = builder.extractTags();

  auto key = std::string{};

  result = put_object(_provider, collection_name, json, oid, 0, tags, key);

  if (!result.first) {
    throw runtime_error("UconDB") << "UconDB failed inserting data, error=" << result.second;
  }

  TLOG(14) << "UconDB::writeDocument() Created document with oid=<" << oid << ">.";

  return {id};
}

namespace ucon {
namespace debug {
void ReadWrite() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::ucon::ReadWrite trace_enable";
}
}  // namespace debug
}  // namespace ucon

}  // namespace database
}  // namespace artdaq
