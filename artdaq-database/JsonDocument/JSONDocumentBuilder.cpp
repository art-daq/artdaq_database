#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

#include <utility>
#include "artdaq-database/JsonDocument/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSNU:DocuBldr_C"

using artdaq::database::ThrowOnFailure;
using artdaq::database::json::value_t;
using artdaq::database::result_t;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;

namespace db = artdaq::database;
namespace utl = db::docrecord;
namespace ovl = db::overlay;

namespace dbdr = artdaq::database::docrecord;

namespace jsonliteral = artdaq::database::dataformats::literal;

JSONDocumentBuilder::JSONDocumentBuilder()
    : _document(std::string(template__empty_document)),
      _overlay(std::make_unique<ovlDatabaseRecord>(_document._value)),
      _initOK(init()) {}

JSONDocumentBuilder::JSONDocumentBuilder(JSONDocument document)
    : _document(std::move(document)),
      _overlay(std::make_unique<ovlDatabaseRecord>(_document._value)),
      _initOK(init()) {}

bool JSONDocumentBuilder::init() {
  auto buff[[gnu::unused]] = _document.writeJson();
  TLOG(13) << "JSONDocumentBuilder::init() new document=<" << buff << ">";
  return true;
}

JSONDocumentBuilder& JSONDocumentBuilder::addAlias(JSONDocument const& alias) try {
  TLOG(13) << "addAlias() args  alias=<" << alias << ">";

  JSONDocument copy(alias);
  auto ovl = overlay<ovl::ovlAlias>(copy, jsonliteral::alias);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->addAlias(ovl));
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TLOG(13) << "addAlias() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::removeAlias(JSONDocument const& alias) try {
  TLOG(14) << "removeAlias() args  alias=<" << alias << ">";

  JSONDocument copy(alias);
  auto ovl = overlay<ovl::ovlAlias>(copy, jsonliteral::alias);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->removeAlias(ovl));
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TLOG(15) << "removeAlias() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::addConfiguration(JSONDocument const& config) try {
  TLOG(15) << "addConfiguration() args config=<" << config << ">";

  JSONDocument copy(config);
  auto ovl = overlay<ovl::ovlConfiguration>(copy, jsonliteral::configuration);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->addConfiguration(ovl));
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TLOG(15) << "addConfiguration() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::removeConfiguration(JSONDocument const& config) try {
  TLOG(15) << "removeConfiguration() args  config=<" << config << ">";

  JSONDocument copy(config);
  auto ovl = overlay<ovl::ovlConfiguration>(copy, jsonliteral::configuration);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->removeConfiguration(ovl));
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TLOG(15) << "removeConfiguration() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::removeAllConfigurations() try {
  TLOG(15) << "removeAllConfigurations()";

  _overlay->configurations().wipe();
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TLOG(15) << "removeAllConfigurations() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());
  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::setObjectID(JSONDocument const& objectId) try {
  TLOG(15) << "setObjectID() args  objectId=<" << objectId << ">";

  JSONDocument copy(objectId);
  auto id = std::make_unique<ovl::ovlId>(jsonliteral::id, copy.findChildValue(jsonliteral::id));

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->swap(id));

  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TLOG(15) << "setObjectID() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocument JSONDocumentBuilder::getObjectID() const { return {_overlay->id().to_string()}; }

std::string JSONDocumentBuilder::getObjectOUID() const { return _overlay->id().oid(); }

bool JSONDocumentBuilder::newObjectID() { return _overlay->id().newId(); }

JSONDocumentBuilder& JSONDocumentBuilder::setVersion(JSONDocument const& version) try {
  TLOG(16) << "setVersion() args  version=<" << version << ">";

  JSONDocument copy(version);

  auto ovl = std::make_unique<ovl::ovlVersion>(jsonliteral::version, copy.findChildValue(jsonliteral::name));

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->setVersion(ovl));
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TLOG(16) << "setVersion() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::setCollection(JSONDocument const& collection) try {
  TLOG(16) << "collection() args  collection=<" << collection << ">";

  auto copy = collection.findChild(jsonliteral::collection);
  auto ovl =
      std::make_unique<ovl::ovlCollection>(jsonliteral::collection, copy.findChildValue(jsonliteral::collection));

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->setCollection(ovl));
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::addEntity(JSONDocument const& entity) try {
  TLOG(19) << "addEntity() args  entity=<" << entity << ">";

  JSONDocument copy(entity);
  auto ovl = overlay<ovl::ovlEntity>(copy, jsonliteral::entity);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->addEntity(ovl));

  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TLOG(19) << "addEntity() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::addRun(JSONDocument const& run) try {
  TLOG(19) << "addRun() args  run=<" << run << ">";

  JSONDocument copy(run);
  auto ovl = overlay<ovl::ovlRun>(copy, jsonliteral::runs);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->addRun(ovl));

  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TLOG(19) << "addRun() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::removeEntity(JSONDocument const& entity) try {
  TLOG(19) << "removeEntity() args  entity=<" << entity << ">";

  JSONDocument copy(entity);
  auto ovl = overlay<ovl::ovlEntity>(copy, jsonliteral::entity);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->removeEntity(ovl));

  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TLOG(19) << "removeEntity() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::removeAllEntities() try {
  TLOG(15) << "removeAllEntities()";

  _overlay->entities().wipe();
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TLOG(15) << "removeAllEntities() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());
  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::markReadonly() try {
  TLOG(16) << "markReadonly()";

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->markReadonly());

  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TLOG(16) << "markReadonly() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::markDeleted() try {
  TLOG(15) << "markDeleted()";

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->markDeleted());

  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TLOG(15) << "markDeleted() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());

  return self();
}

bool JSONDocumentBuilder::isReadonlyOrDeleted() const { return _overlay->isReadonlyOrDeleted(); }

std::list<std::string> JSONDocumentBuilder::extractTags() const {
  auto retValue = std::list<std::string>{};

  retValue.push_back(std::string(jsonliteral::version).append(":").append(_overlay->version().string_value()));

  auto const& configurations = _overlay->configurations();

  for (auto const& configuration : configurations) {
    retValue.push_back(std::string(jsonliteral::configuration).append(":").append(configuration.name()));
  }

  auto const& entities = _overlay->entities();

  for (auto const& entity : entities) {
    retValue.push_back(std::string(jsonliteral::entity).append(":").append(entity.name()));
  }

  return retValue;
}

result_t JSONDocumentBuilder::comapreUsingOverlays(JSONDocumentBuilder const& other) const {
  return *_overlay == *other._overlay;
}

std::string JSONDocumentBuilder::to_string() const { return _document.to_string(); }

std::ostream& utl::operator<<(std::ostream& os, JSONDocumentBuilder const& data) {
  os << data.to_string();
  return os;
}

void dbdr::debug::JSONDocumentBuilder() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::JSONDocumentBuilder trace_enable";
}