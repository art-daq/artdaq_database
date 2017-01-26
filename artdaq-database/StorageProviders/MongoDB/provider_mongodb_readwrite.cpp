#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb_headers.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif
#define TRACE_NAME "MongoDB:RDWRT_C"

namespace artdaq {
namespace database {

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::readDocument(JsonData const& arg) {
  TRACE_(3, "MongoDB::readDocument() begin");
  TRACE_(3, "MongoDB::readDocument() args=<" << arg.json_buffer << ">");

  auto returnCollection = std::list<JsonData>();

  auto arg_document = JSONDocument{arg.json_buffer};

  auto filter_document = arg_document.findChildDocument(jsonliteral::filter);

  auto collection_name = std::string{};

  try {
    collection_name = filter_document.findChild(jsonliteral::collection).value();
  } catch (...) {
    TRACE_(3, "MongoDB::readDocument() Filter must have the collection element.");
  }

  if (collection_name.empty()) collection_name = arg_document.findChild(jsonliteral::collection).value();

  confirm(!collection_name.empty());

  TRACE_(3, "MongoDB::readDocument() collection_name=<" << collection_name << ">");

  auto collection = _provider->connection().collection(collection_name);

  auto filter_json = filter_document.to_string();

  TRACE_(3, "MongoDB::readDocument() query_payload=<" << filter_json << ">");

  auto filter_bsondoc = bsoncxx::from_json(filter_json);

  auto size = collection.count(filter_bsondoc.view());

  TRACE_(3, "MongoDB::readDocument() Found " << size << " document(s).");

  auto cursor = collection.find(filter_bsondoc.view());

  for (auto& doc : cursor) {
    TRACE_(3, "MongoDB::readDocument() found_document=<" << artdaq::database::mongo::to_json(doc) << ">");

    returnCollection.emplace_back(artdaq::database::mongo::to_json(doc));
  }
  return returnCollection;
}

template <>
object_id_t StorageProvider<JsonData, MongoDB>::writeDocument(JsonData const& arg) {
  TRACE_(4, "MongoDB::writeDocument() begin");
  TRACE_(4, "MongoDB::writeDocument() args=<" << arg.json_buffer << ">");

  auto arg_document = JSONDocument{arg.json_buffer};

  auto user_document = arg_document.findChildDocument(jsonliteral::document);

  auto filter_document = JSONDocument{};

  try {
    filter_document = arg_document.findChildDocument(jsonliteral::filter);
  } catch (...) {
    TRACE_(4, "MongoDB::writeDocument() No filter was found.");
  }

  auto collection_name = std::string{};

  try {
    collection_name = user_document.findChild(jsonliteral::collection).value();
  } catch (...) {
    TRACE_(4, "MongoDB::writeDocument() User document must have the collection element.");
  }

  if (collection_name.empty()) try {
      collection_name = filter_document.findChild(jsonliteral::collection).value();
    } catch (...) {
      TRACE_(4, "MongoDB::writeDocument() Filter should have the collection element.");
    }

  if (collection_name.empty()) collection_name = arg_document.findChild(jsonliteral::collection).value();

  confirm(!collection_name.empty());

  TRACE_(4, "MongoDB::writeDocument() collection_name=<" << collection_name << ">");

  auto oid = object_id_t{ouid_invalid};

  auto isNew = bool{true};

  try {
    auto oid_json = filter_document.findChild(jsonliteral::id).value();
    TRACE_(4, "MongoDB::writeDocument() Found filter=<" << oid_json << ">");
    oid = extract_oid(oid_json);
    isNew = false;
    TRACE_(4, "MongoDB::writeDocument() Using provided oid=<" << oid << ">");
  } catch (...) {
        TRACE_(4, "MongoDB::writeDocument() No filter found; filter_document =<" << filter_document << ">");
  }

  if (oid == object_id_t{ouid_invalid}) {
    oid = generate_oid();
    TRACE_(4, "MongoDB::writeDocument() Using generated oid=<" << oid << ">");    
  }

  auto id = to_id(oid);

  JSONDocumentBuilder builder(user_document);
  builder.setObjectID({id});
  builder.setCollection({to_json(jsonliteral::collection, collection_name)});

  auto user_bsondoc = bsoncxx::from_json(builder.to_string());

  auto collection = _provider->connection().collection(collection_name);

  auto filter_json = filter_document.to_string();

  auto filter_bsondoc = bsoncxx::from_json(filter_json);

  if (isNew) {
    auto result = collection.insert_one(user_bsondoc.view());

    oid = extract_oid(bsoncxx::to_json(result->inserted_id()));

    id = to_id(oid);

    TRACE_(4, "MongoDB::writeDocument() Inserted _id=<" << id << ">");

    return id;
  }

  auto size = collection.count(filter_bsondoc.view());
  TRACE_(4, "MongoDB::writeDocument() Found " << size << " document(s).");

  if (size > 1)
    throw runtime_error("MongoDB") << "MongoDB failed inserting data, search filter is too wide; filter= <"
                                   << filter_json << ">, returned count=" << size << ".";

  auto result = collection.replace_one(filter_bsondoc.view(), user_bsondoc.view());

  TRACE_(4, "MongoDB::writeDocument() Modified " << result->modified_count() << "document.");

  return id;
}

namespace mongo {
namespace debug {
void enableReadWrite() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::mongo::enableReadWrite trace_enable");
}
}
}  // namespace mongo

}  // namespace database
}  // namespace artdaq
