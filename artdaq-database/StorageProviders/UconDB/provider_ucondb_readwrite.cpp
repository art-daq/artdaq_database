#include "artdaq-database/StorageProviders/UconDB/provider_ucondb_headers.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "UconDB:RDWRT_C"

namespace artdaq {
namespace database {
template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::readDocument(JsonData const& arg) {
  TRACE_(3, "UconDB::readDocument() begin");
  TRACE_(3, "UconDB::readDocument() args=<" << arg.json_buffer << ">");

  auto returnCollection = std::list<JsonData>();

  auto arg_document = JSONDocument{arg.json_buffer};

  auto filter_document = JSONDocument{};

  try {
    filter_document = arg_document.findChildDocument(jsonliteral::filter);
  } catch (...) {
    TRACE_(3, "UconDB::readDocument() No filter was found.");
  }

  auto collection_name = std::string{};

  try {
    collection_name = filter_document.findChild(jsonliteral::collection).value();
  } catch (...) {
    TRACE_(3, "UconDB::readDocument() Filter must have the collection element.");
  }

  if (collection_name.empty()) collection_name = arg_document.findChild(jsonliteral::collection).value();

  confirm(!collection_name.empty());

  TRACE_(3, "UconDB::readDocument() collection_name=<" << collection_name << ">");


  auto filter_json = filter_document.to_string();

  TRACE_(3, "UconDB::readDocument() filter_json=<" << filter_json << ">.");


  return returnCollection;
}

template <>
object_id_t StorageProvider<JsonData, UconDB>::writeDocument(JsonData const& arg) {
  TRACE_(4, "UconDB::writeDocument() begin");
  TRACE_(4, "UconDB::writeDocument() args=<" << arg.json_buffer << ">");

  auto arg_document = JSONDocument{arg.json_buffer};

  auto user_document = arg_document.findChildDocument(jsonliteral::document);

  auto filter_document = JSONDocument{};

  try {
    filter_document = arg_document.findChildDocument(jsonliteral::filter);
  } catch (...) {
    TRACE_(4, "UconDB::writeDocument() No filter was found.");
  }

  auto collection_name = std::string{};

  try {
    collection_name = user_document.findChild(jsonliteral::collection).value();
  } catch (...) {
    TRACE_(4, "UconDB::writeDocument() User document must have the collection element.");
  }

  if (collection_name.empty()) try {
      collection_name = filter_document.findChild(jsonliteral::collection).value();
    } catch (...) {
      TRACE_(4, "UconDB::writeDocument() Filter should have the collection element.");
    }

  if (collection_name.empty()) collection_name = arg_document.findChild(jsonliteral::collection).value();

  confirm(!collection_name.empty());

  TRACE_(4, "UconDB::writeDocument() collection_name=<" << collection_name << ">");

  auto oid = object_id_t{ouid_invalid};

  auto isNew = bool{true};

  try {
    auto oid_json = filter_document.findChild(jsonliteral::id).value();
    TRACE_(4, "UconDB::writeDocument() Found filter=<" << oid_json << ">");
    oid = extract_oid(oid_json);
    isNew = false;
    TRACE_(4, "UconDB::writeDocument() Using provided oid=<" << oid << ">");
  } catch (...) {
  }

  if (oid == object_id_t{ouid_invalid}) {
    oid = generate_oid();
    TRACE_(4, "UconDB::writeDocument() Using generated oid=<" << oid << ">");    
  }


  auto id = to_id(oid);

  JSONDocumentBuilder builder(user_document);
  builder.setObjectID({id});
  builder.setCollection({to_json(jsonliteral::collection, collection_name)});

  auto json = builder.to_string();

  TRACE_(4, "UconDB::writeDocument() json=<" << json << ">.");
  
  auto res= folders(_provider);

  for( auto & c:res) {
    tags(_provider,c);
    auto objs=objects(_provider,c);
    for( auto & o:objs) {
      get_object(_provider,c,o);
    }
  }
  
  if(isNew) {
    TRACE_(4, "UconDB::writeDocument() isnew");
  }
  
  return {id};
}

namespace ucon {
namespace debug {
void enableReadWrite() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::ucon::enableReadWrite trace_enable");
}
}
}  // namespace ucon

}  // namespace database
}  // namespace artdaq
