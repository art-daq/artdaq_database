#include "artdaq-database/JsonDocument/JSONDocumentMigrator.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSONDocumentMigrator.cpp"

using artdaq::database::json::array_t;
using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::result_t;

using artdaq::database::overlay::ovlDatabaseRecordUPtr_t;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::docrecord::JSONDocumentMigrator;

namespace db = artdaq::database;

namespace dbdr = artdaq::database::docrecord;

namespace jsonliteral = artdaq::database::dataformats::literal;

JSONDocumentMigrator::JSONDocumentMigrator(JSONDocument const& document) : _document(document) {}

JSONDocumentMigrator::operator JSONDocument() const {
  JSONDocumentBuilder builder{};

  builder.createFromData(_document);

  auto version = jsn::object_t{};
  version[jsonliteral::name] = _document.findChildValue(jsonliteral::version);
  builder.setVersion({version});

  auto entity = _document.findChildDocument("configurable_entity");
  builder.addEntity(entity);

  jsn::value_t configs = _document.findChildValue(jsonliteral::configurations);

  for (auto const& config : unwrap(configs).value_as<jsn::array_t>()) {
    builder.addConfiguration({config});
  }

  jsn::value_t oid = _document.findChildValue("_id._oid");
  builder.setObjectID(db::to_id(unwrap(oid).value_as<std::string>()));

  return builder.extract();
}

void dbdr::debug::JSONDocumentMigrator() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::JSONDocumentMigrator trace_enable";
}