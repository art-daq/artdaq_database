#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSNU:DocuBldr_C"

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::json::array_t;
using artdaq::database::json::type_t;
using artdaq::database::ThrowOnFailure;
using artdaq::database::result_t;

using namespace artdaq::database::docrecord;

namespace db = artdaq::database;
namespace utl = db::docrecord;
namespace ovl = db::overlay;

namespace dbdr = artdaq::database::docrecord;

namespace jsonliteral = artdaq::database::dataformats::literal;

JSONDocumentBuilder::JSONDocumentBuilder()
    : _document(std::string(template__empty_document)),
      _overlay(std::make_unique<ovlDatabaseRecord>(_document._value)),
      _initOK(init()) {}

JSONDocumentBuilder::JSONDocumentBuilder(JSONDocument const& document)
    : _document(document), _overlay(std::make_unique<ovlDatabaseRecord>(_document._value)), _initOK(init()) {}

bool JSONDocumentBuilder::init() {
  auto buff[[gnu::unused]] = _document.writeJson();
  TRACE_(3, "JSONDocumentBuilder::init() new document=<" << buff << ">");
  return true;
}

JSONDocumentBuilder& JSONDocumentBuilder::addAlias(JSONDocument const& alias) try {
  TRACE_(3, "addAlias() args  alias=<" << alias << ">");

  JSONDocument copy(alias);
  auto ovl = overlay<ovl::ovlAlias>(copy, jsonliteral::alias);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->addAlias(ovl));
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TRACE_(3, "addAlias() Exception:" << ex.what());
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::removeAlias(JSONDocument const& alias) try {
  TRACE_(4, "removeAlias() args  alias=<" << alias << ">");

  JSONDocument copy(alias);
  auto ovl = overlay<ovl::ovlAlias>(copy, jsonliteral::alias);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->removeAlias(ovl));
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TRACE_(5, "removeAlias() Exception:" << ex.what());
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::addConfiguration(JSONDocument const& config) try {
  TRACE_(5, "addConfiguration() args config=<" << config << ">");

  JSONDocument copy(config);
  auto ovl = overlay<ovl::ovlConfiguration>(copy, jsonliteral::configuration);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->addConfiguration(ovl));
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TRACE_(5, "addConfiguration() Exception:" << ex.what());
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::removeConfiguration(JSONDocument const& config) try {
  TRACE_(5, "removeConfiguration() args  config=<" << config << ">");

  JSONDocument copy(config);
  auto ovl = overlay<ovl::ovlConfiguration>(copy, jsonliteral::configuration);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->removeConfiguration(ovl));
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TRACE_(5, "removeConfiguration() Exception:" << ex.what());
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::setObjectID(JSONDocument const& objectId) try {
  TRACE_(8, "setObjectID() args  objectId=<" << objectId << ">");

  JSONDocument copy(objectId);
  auto id = std::make_unique<ovl::ovlId>(jsonliteral::id, copy.findChildValue(jsonliteral::id));
  
  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->swap(id));

  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TRACE_(5, "setObjectID() Exception:" << ex.what());
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::setVersion(JSONDocument const& version) try {
  TRACE_(6, "setVersion() args  version=<" << version << ">");

  JSONDocument copy(version);

  auto ovl = std::make_unique<ovl::ovlVersion>(jsonliteral::version, copy.findChildValue(jsonliteral::name));

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->setVersion(ovl));
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TRACE_(6, "setVersion() Exception:" << ex.what());
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::setCollection(JSONDocument const& collection) try {
  TRACE_(6, "collection() args  collection=<" << collection << ">");

  auto copy =collection.findChild(jsonliteral::collection);
  auto ovl = std::make_unique<ovl::ovlCollection>(jsonliteral::collection, copy.findChildValue(jsonliteral::collection));

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->setCollection(ovl));
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TRACE_(6, "collection() Exception:" << ex.what());
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::addEntity(JSONDocument const& entity) try {
  TRACE_(9, "addEntity() args  entity=<" << entity << ">");

  JSONDocument copy(entity);
  auto ovl = overlay<ovl::ovlConfigurableEntity>(copy, jsonliteral::entities);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->addEntity(ovl));

  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TRACE_(9, "setVersion() Exception:" << ex.what());
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::removeEntity(JSONDocument const& entity) try {
  TRACE_(9, "addEntity() args  entity=<" << entity << ">");

  JSONDocument copy(entity);
  auto ovl = overlay<ovl::ovlConfigurableEntity>(copy, jsonliteral::entities);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->removeEntity(ovl));

  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TRACE_(9, "setVersion() Exception:" << ex.what());
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::markReadonly() try {
  TRACE_(6, "markReadonly()");

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->markReadonly());

  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TRACE_(6, "markReadonly() Exception:" << ex.what());
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::markDeleted() try {
  TRACE_(7, "markDeleted()");

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->markDeleted());

  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TRACE_(7, "markDeleted() Exception:" << ex.what());
  ThrowOnFailure(CallUndo());

  return self();
}

result_t JSONDocumentBuilder::comapreUsingOverlays(JSONDocumentBuilder const& other) const {
  return *_overlay == *other._overlay;
}

std::string JSONDocumentBuilder::to_string() const { return _document.to_string(); }

std::ostream& utl::operator<<(std::ostream& os, JSONDocumentBuilder const& data) {
  os << data.to_string();
  return os;
}

void dbdr::debug::enableJSONDocumentBuilder() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::JSONDocumentBuilder trace_enable");
}