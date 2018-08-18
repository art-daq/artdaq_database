#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb_headers.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif
#define TRACE_NAME "provider_mongodb_readwrite.cpp"

#include <mongocxx/options/update.hpp>

namespace artdaq {
namespace database {

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, MongoDB>::readDocument(JSONDocument const& arg) {
  TLOG(13) << "MongoDB::readDocument() begin";

  auto returnCollection = std::vector<JSONDocument>();

  auto filter_document = arg.findChildDocument(jsonliteral::filter);

  auto collection_name = std::string{};

  try {
    collection_name = filter_document.findChild(jsonliteral::collection).value();
  } catch (...) {
    TLOG(13) << "MongoDB::readDocument() Filter must have the collection element.";
  }

  if (collection_name.empty()) {
    collection_name = arg.findChild(jsonliteral::collection).value();
  }

  confirm(!collection_name.empty());

  TLOG(13) << "MongoDB::readDocument() collection_name=<" << collection_name << ">";

  auto collection = _provider->connection().collection(collection_name);

  auto filter_json = filter_document.to_string();

  TLOG(13) << "MongoDB::readDocument() query_payload=<" << filter_json << ">";

  auto filter_bsondoc = compat::from_json(filter_json);

  auto size = collection.count(filter_bsondoc.view());

  TLOG(13) << "MongoDB::readDocument() Found " << size << " document(s).";

  auto cursor = collection.find(filter_bsondoc.view());

  for (auto& doc : cursor) {
    TLOG(13) << "MongoDB::readDocument() found_document=<" << compat::to_json_unescaped(doc) << ">";

    returnCollection.emplace_back(compat::to_json_unescaped(doc));
  }
  return returnCollection;
}

template <>
object_id_t StorageProvider<JSONDocument, MongoDB>::writeDocument(JSONDocument const& arg) {
  TLOG(14) << "MongoDB::writeDocument() begin";

  auto user_document = arg.findChildDocument(jsonliteral::document);

  auto filter_document = JSONDocument{};

  try {
    filter_document = arg.findChildDocument(jsonliteral::filter);
  } catch (...) {
    TLOG(14) << "MongoDB::writeDocument() No filter was found.";
  }

  auto collection_name = std::string{};

  try {
    collection_name = user_document.findChild(jsonliteral::collection).value();
  } catch (...) {
    TLOG(14) << "MongoDB::writeDocument() User document must have the collection "
                "element.";
  }

  if (collection_name.empty()) {
    try {
      collection_name = filter_document.findChild(jsonliteral::collection).value();
    } catch (...) {
      TLOG(14) << "MongoDB::writeDocument() Filter should have the collection "
                  "element.";
    }
  }

  if (collection_name.empty()) {
    collection_name = arg.findChild(jsonliteral::collection).value();
  }

  confirm(!collection_name.empty());

  TLOG(14) << "MongoDB::writeDocument() collection_name=<" << collection_name << ">";

  auto oid = object_id_t{ouid_invalid};

  auto isNew = bool{true};

  try {
    auto oid_json = filter_document.findChild(jsonliteral::id).value();
    TLOG(14) << "MongoDB::writeDocument() Found filter=<" << oid_json << ">";
    oid = extract_oid(oid_json);
    isNew = false;
    TLOG(14) << "MongoDB::writeDocument() Using provided oid=<" << oid << ">";
  } catch (...) {
    TLOG(14) << "MongoDB::writeDocument() No filter found; filter_document =<" << filter_document << ">";
  }

  if (oid == object_id_t{ouid_invalid}) {
    oid = generate_oid();
    TLOG(14) << "MongoDB::writeDocument() Using generated oid=<" << oid << ">";
  }

  auto id = to_id(oid);

  JSONDocumentBuilder builder(user_document);
  builder.setObjectID({id});
  builder.setCollection({to_json(jsonliteral::collection, collection_name)});

  auto collection = _provider->connection().collection(collection_name);

  auto filter_json = filter_document.to_string();

  auto filter_bsondoc = compat::from_json(filter_json);

  auto found = collection.count(filter_bsondoc.view());

  if (!isNew && found == 0) {
    isNew = true;
  }

  if (isNew) {
    auto user_bsondoc = compat::from_json(builder.to_string());
    auto result = collection.insert_one(user_bsondoc.view());
    oid = extract_oid(compat::to_json(result->inserted_id()));
    id = to_id(oid);
    TLOG(14) << "MongoDB::writeDocument() Inserted _id=<" << id << ">";

    return id;
  }

  TLOG(14) << "MongoDB::writeDocument() Found " << found << " document(s).";

  if (found > 1) {
    throw runtime_error("MongoDB") << "MongoDB failed inserting data, search filter is too wide; filter= <"
                                   << filter_json << ">, returned count=" << found << ".";
  }
  {
    auto user_doc = builder.extract();
    user_doc.deleteChild(jsonliteral::id);
    auto user_bsondoc = compat::from_json(user_doc.to_string());
    auto result = collection.insert_one(user_bsondoc.view());
    oid = extract_oid(compat::to_json(result->inserted_id()));
    id = to_id(oid);

    TLOG(14) << "MongoDB::writeDocument() Inserted _id=<" << id << ">";
  }

  TLOG(14) << "MongoDB::writeDocument() Deleting documents matching filter=<" << compat::to_json(filter_bsondoc.view())
           << ">";

  auto result = collection.delete_one(filter_bsondoc.view());

  TLOG(14) << "MongoDB::writeDocument() Deleted " << result->deleted_count() << " document(s).";

  return id;
}

namespace mongo {
namespace debug {
void ReadWrite() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::mongo::ReadWrite trace_enable";
}
}  // namespace debug
}  // namespace mongo

}  // namespace database
}  // namespace artdaq
