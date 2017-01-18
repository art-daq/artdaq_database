#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_headers.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "FileDB:RDWRT_C"

namespace artdaq {
namespace database {
template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::readConfiguration(JsonData const& arg) {
  TRACE_(3, "FileSystemDB::readConfiguration() begin");
  TRACE_(3, "FileSystemDB::readConfiguration() args=<" << arg.json_buffer << ">");

  auto returnCollection = std::list<JsonData>();

  auto arg_document = JSONDocument{arg.json_buffer};

  auto filter_document = JSONDocument{};

  try {
    filter_document = arg_document.findChildDocument(jsonliteral::filter);
  } catch (...) {
    TRACE_(3, "FileSystemDB::readConfiguration() No filter was found.");
  }

  auto collection_name = std::string{};

  try {
    collection_name = filter_document.findChild(jsonliteral::collection).value();
  } catch (...) {
    TRACE_(3, "FileSystemDB::readConfiguration() Filter must have the collection element.");
  }

  if (collection_name.empty()) collection_name = arg_document.findChild(jsonliteral::collection).value();

  confirm(!collection_name.empty());

  TRACE_(3, "FileSystemDB::readConfiguration() collection_name=<" << collection_name << ">");

  auto collection = _provider->connection() + collection_name;

  TRACE_(3, "FileSystemDB::readConfiguration() collection_path=<" << collection << ">");

  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection);

  auto filter_json = filter_document.to_string();

  auto index_path = boost::filesystem::path(dir_name.c_str()).append(dbfsl::search_index);

  SearchIndex search_index(index_path);

  TRACE_(3, "FileSystemDB::readConfiguration() filter_json=<" << filter_json << ">.");

  auto oids = search_index.findDocumentIDs(filter_json);

  TRACE_(3, "FileSystemDB::readConfiguration() search returned " << oids.size() << " documents.");

  for (auto const& oid : oids) {
    auto doc_path = boost::filesystem::path(dir_name.c_str()).append(oid).replace_extension(".json");

    std::ifstream is(doc_path.c_str());
    std::string json((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

    is.close();

    TRACE_(3, "FileSystemDB::readConfiguration() found_document=<" << json << ">");

    returnCollection.emplace_back(json);
  }

  return returnCollection;
}

template <>
object_id_t StorageProvider<JsonData, FileSystemDB>::writeConfiguration(JsonData const& arg) {
  TRACE_(4, "FileSystemDB::writeConfiguration() begin");
  TRACE_(4, "FileSystemDB::writeConfiguration() args=<" << arg.json_buffer << ">");

  auto arg_document = JSONDocument{arg.json_buffer};

  auto user_document = arg_document.findChildDocument(jsonliteral::document);

  auto filter_document = JSONDocument{};

  try {
    filter_document = arg_document.findChildDocument(jsonliteral::filter);
  } catch (...) {
    TRACE_(4, "FileSystemDB::writeConfiguration() No filter was found.");
  }

  auto collection_name = std::string{};

  try {
    collection_name = user_document.findChild(jsonliteral::collection).value();
  } catch (...) {
    TRACE_(4, "FileSystemDB::writeConfiguration() User document must have the collection element.");
  }

  if (collection_name.empty()) try {
      collection_name = filter_document.findChild(jsonliteral::collection).value();
    } catch (...) {
      TRACE_(4, "FileSystemDB::writeConfiguration() Filter should have the collection element.");
    }

  if (collection_name.empty()) collection_name = arg_document.findChild(jsonliteral::collection).value();

  confirm(!collection_name.empty());

  TRACE_(4, "FileSystemDB::writeConfiguration() collection_name=<" << collection_name << ">");

  auto oid = object_id_t{ouid_invalid};

  auto isNew = bool{true};

  try {
    auto oid_json = filter_document.findChild(jsonliteral::id).value();
    TRACE_(4, "FileSystemDB::writeConfiguration() Found filter=<" << oid_json << ">");
    oid = extract_oid(oid_json);
    isNew = false;
    TRACE_(4, "FileSystemDB::writeConfiguration() Using provided oid=<" << oid << ">");
  } catch (...) {
  }

  if (oid == object_id_t{ouid_invalid}) {
    oid = generate_oid();
    TRACE_(4, "FileSystemDB::writeConfiguration() Using generated oid=<" << oid << ">");    
  }


  auto id = to_id(oid);

  JSONDocumentBuilder builder(user_document);
  builder.setObjectID({id});
  builder.setCollection({to_json(jsonliteral::collection, collection_name)});

  auto collection = _provider->connection() + collection_name;

  collection = expand_environment_variables(collection);

  TRACE_(4, "FileSystemDB::writeConfiguration() collection_path=<" << collection << ">");

  auto filename = dbfs::mkdir(collection) + oid + ".json";

  TRACE_(4, "FileSystemDB::writeConfiguration() filename=<" << filename << ">.");

  if (isNew) {
    if (dbfs::check_if_file_exists(filename))
      throw runtime_error("FileSystemDB") << "FileSystemDB failed inserting data, document already exist; filename= <"
                                          << filename << ">, filter= <" << filter_document << ">";
  }

  std::ofstream os(filename);

  auto json = builder.to_string();

  TRACE_(4, "FileSystemDB::writeConfiguration() json=<" << json << ">.");
  
  std::copy(json.begin(), json.end(), std::ostream_iterator<char>(os));

  os.close();

  auto index_path = boost::filesystem::path(filename.c_str()).parent_path().append(dbfsl::search_index);

  SearchIndex search_index(index_path);

  if (!search_index.addDocument(json, oid)) {
    TRACE_(4, "FileSystemDB::writeConfiguration() Failed updating SearchIndex.");
  }

  return {id};
}

namespace filesystem {
namespace debug {
void enableReadWrite() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::filesystem::enableReadWrite trace_enable");

  artdaq::database::filesystem::index::debug::enable();
}
}
}  // namespace filesystem

}  // namespace database
}  // namespace artdaq
