#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_headers.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "provider_filedb_readwrite.cpp"

namespace artdaq {
namespace database {
template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::readDocument(JSONDocument const& arg) {
  TLOG(13) << "FileSystemDB::readDocument() begin";

  auto returnCollection = std::vector<JSONDocument>();

  auto filter_document = JSONDocument{};

  try {
    filter_document = arg.findChildDocument(jsonliteral::filter);
  } catch (...) {
    TLOG(13) << "FileSystemDB::readDocument() No filter was found.";
  }

  auto collection_name = std::string{};

  try {
    collection_name = filter_document.findChild(jsonliteral::collection).value();
  } catch (...) {
    TLOG(13) << "FileSystemDB::readDocument() Filter must have the collection element.";
  }

  if (collection_name.empty()) {
    collection_name = arg.findChild(jsonliteral::collection).value();
  }

  confirm(!collection_name.empty());

  TLOG(13) << "FileSystemDB::readDocument() collection_name=<" << collection_name << ">";

  auto collection = _provider->connection() + collection_name;

  TLOG(13) << "FileSystemDB::readDocument() collection_path=<" << collection << ">";

  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection);

  auto filter_json = filter_document.to_string();

  auto index_path = boost::filesystem::path(dir_name.c_str()).append("/").append(dbfsl::search_index);

  SearchIndex search_index(index_path);

  TLOG(13) << "FileSystemDB::readDocument() filter_json=<" << filter_json << ">.";

  auto oids = search_index.findDocumentIDs(filter_json);

  TLOG(13) << "FileSystemDB::readDocument() search returned " << oids.size() << " documents.";

  for (auto const& oid : oids) {
    auto doc_path = boost::filesystem::path(dir_name.c_str()).append(oid).replace_extension(".json");

    auto json = std::string{};
    db::read_buffer_from_file(json, {doc_path.c_str()});

    TLOG(13) << "FileSystemDB::readDocument() found_document=<" << json << ">";

    returnCollection.emplace_back(json);
  }

  return returnCollection;
}

template <>
object_id_t StorageProvider<JSONDocument, FileSystemDB>::writeDocument(JSONDocument const& arg) {
  TLOG(14) << "FileSystemDB::writeDocument() begin";

  auto user_document = arg.findChildDocument(jsonliteral::document);

  auto filter_document = JSONDocument{};

  try {
    filter_document = arg.findChildDocument(jsonliteral::filter);
  } catch (...) {
    TLOG(14) << "FileSystemDB::writeDocument() No filter was found.";
  }

  auto collection_name = std::string{};

  try {
    collection_name = user_document.findChild(jsonliteral::collection).value();
  } catch (...) {
    TLOG(14) << "FileSystemDB::writeDocument() User document must have the collection element.";
  }

  if (collection_name.empty()) {
    try {
      collection_name = filter_document.findChild(jsonliteral::collection).value();
    } catch (...) {
      TLOG(14) << "FileSystemDB::writeDocument() Filter should have the collection element.";
    }
  }

  if (collection_name.empty()) {
    collection_name = arg.findChild(jsonliteral::collection).value();
  }

  confirm(!collection_name.empty());

  TLOG(14) << "FileSystemDB::writeDocument() collection_name=<" << collection_name << ">";

  auto oid = object_id_t{ouid_invalid};

  auto isNew = bool{true};

  try {
    auto oid_json = filter_document.findChild(jsonliteral::id).value();
    TLOG(14) << "FileSystemDB::writeDocument() Found filter=<" << oid_json << ">";
    oid = extract_oid(oid_json);
    isNew = false;
    TLOG(14) << "FileSystemDB::writeDocument() Using provided oid=<" << oid << ">";
  } catch (...) {
  }

  if (oid == object_id_t{ouid_invalid}) {
    oid = generate_oid();
    TLOG(14) << "FileSystemDB::writeDocument() Using generated oid=<" << oid << ">";
  }

  auto id = to_id(oid);

  JSONDocumentBuilder builder(user_document);
  builder.setObjectID({id});
  builder.setCollection({to_json(jsonliteral::collection, collection_name)});

  auto collection = _provider->connection() + collection_name;

  collection = expand_environment_variables(collection);

  TLOG(14) << "FileSystemDB::writeDocument() collection_path=<" << collection << ">";

  auto dir_name = dbfs::mkdir(collection);
  auto filename = dir_name + "/" + oid + ".json";

  TLOG(14) << "FileSystemDB::writeDocument() filename=<" << filename << ">.";

  if (isNew) {
    if (dbfs::check_if_file_exists(filename)) {
      throw runtime_error("FileSystemDB") << "FileSystemDB failed inserting data, document already exist; filename= <" << filename << ">, filter= <"
                                          << filter_document << ">";
    }

    try {
      auto version_value = user_document.findChild(jsonliteral::version).value();

      auto entity_value = std::string{};
      try {
        auto entities_doc = user_document.findChildDocument(jsonliteral::entities);
        auto entity_item = JSONDocument::value_at(entities_doc, 0);
        auto entity_json = JSONDocument{entity_item};
        entity_value = entity_json.findChild(jsonliteral::name).value();
      } catch (...) {
        TLOG(14) << "FileSystemDB::writeDocument() Could not extract entity name";
      }

      TLOG(14) << "FileSystemDB::writeDocument() Checking for duplicate: version=<" << version_value << ">, entity=<" << entity_value << ">";

      std::ostringstream filter_oss;
      filter_oss << "{\"" << apiliteral::filter::version << "\": \"" << version_value << "\"";
      if (!entity_value.empty()) {
        filter_oss << ", \"" << apiliteral::filter::entities << "\": \"" << entity_value << "\"";
      }
      filter_oss << "}";

      auto index_path = boost::filesystem::path(dir_name.c_str()).append(dbfsl::search_index);
      SearchIndex search_index(index_path);

      auto existing_oids = search_index.findDocumentIDs(JSONDocument{filter_oss.str()});

      if (!existing_oids.empty()) {
        throw runtime_error("FileSystemDB") << "Document with same version and entity already exists; "
                                            << "collection=<" << collection_name << ">, "
                                            << "version=<" << version_value << ">, "
                                            << "entity=<" << entity_value << ">";
      }

      TLOG(14) << "FileSystemDB::writeDocument() No duplicate found, proceeding with write";
    } catch (runtime_error const&) {
      throw;
    } catch (...) {
      TLOG(14) << "FileSystemDB::writeDocument() Could not check for semantic duplicates, proceeding with write";
    }
  }

  auto json = builder.to_string();

  TLOG(14) << "FileSystemDB::writeDocument() json=<" << json << ">.";

  auto index_path = boost::filesystem::path(filename.c_str()).parent_path().append(dbfsl::search_index);

  SearchIndex search_index(index_path);

  if (!isNew) {
    auto old_json = std::string{};
    if (dbfs::check_if_file_exists(filename)) {
      db::read_buffer_from_file(old_json, {filename});
      if (!old_json.empty()) {
        TLOG(14) << "FileSystemDB::writeDocument() Removing old document from index.";
        search_index.removeDocument(JSONDocument{old_json}, oid);
      }
    }
  }

  db::write_buffer_to_file(json, filename);

  if (!search_index.addDocument(json, oid)) {
    TLOG(14) << "FileSystemDB::writeDocument() Failed updating SearchIndex.";
  }

  return {id};
}

namespace filesystem {
namespace debug {
void ReadWrite() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::filesystem::ReadWrite trace_enable";

  artdaq::database::filesystem::index::debug::enable();
}
}  // namespace debug
}  // namespace filesystem

}  // namespace database
}  // namespace artdaq
