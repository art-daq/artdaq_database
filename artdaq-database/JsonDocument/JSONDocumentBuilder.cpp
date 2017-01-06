#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/common.h"

#include <ctime>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSNU:DocuBldr_C"

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::json::array_t;
using artdaq::database::json::type_t;
using artdaq::database::ThrowOnFailure;

using namespace artdaq::database::jsonutils;

namespace db = artdaq::database;
namespace utl = db::jsonutils;
namespace ovl = db::overlay;

constexpr auto name = "\"name\"";
constexpr auto assigned = "\"assigned\"";
constexpr auto colon = ":";
constexpr auto quote = "\"";
constexpr auto comma = ",";

JSONDocumentBuilder& JSONDocumentBuilder::createFromData(JSONDocument const& document) {
  
  _overlay.reset(nullptr);
  
  TRACE_(2, "createFrom() args  document=<" << document << ">");

  _createFromTemplate(JSONDocument(std::string(template__default_document)));

  // replace metadata if any
  try {
    auto metadata = document.findChild(literal::metadata);

    TRACE_(2, "createFrom() Found document.metadata=<" << metadata << ">");

    _document.replaceChild(metadata, literal::metadata);

  } catch (notfound_exception const&) {
    TRACE_(2, "createFrom() No document.metadata");
  }

  // replace data if any
  try {
    auto data = document.findChild(literal::changelog);

    TRACE_(2, "createFrom() Found converted.changelog=<" << data << ">");

    _document.replaceChild(data, literal::changelog);

  } catch (notfound_exception const&) {
    TRACE_(2, "createFrom() No converted.changelog");
  }

  // replace data origin if any
  try {
    auto data = document.findChild(literal::origin);

    TRACE_(2, "createFrom() Found origin=<" << data << ">");

    _document.replaceChild(data, literal::origin);

  } catch (notfound_exception const&) {
    TRACE_(2, "createFrom() No origin");
  }

  // replace data if any
  try {
    auto data = document.findChild(literal::data);

    TRACE_(2, "createFrom() Found document.data=<" << data << ">");

    _document.replaceChild(data, literal::data);

    return self();
  } catch (notfound_exception const&) {
    TRACE_(2, "createFrom() No document.data");
  }

  // document contains data only
  _document.replaceChild(document, literal::data);

  auto ovl = std::make_unique<ovlDatabaseRecord>(_document._value);

  _document.writeJson();
  
  std::swap(_overlay,ovl);
  
  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::addAlias(JSONDocument const& alias) try {
  TRACE_(3, "addAlias() args  alias=<" << alias << ">");

  JSONDocument copy(alias);
  auto ovl = overlay<ovl::ovlAlias>(copy, literal::alias);

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
  auto ovl = overlay<ovl::ovlAlias>(copy, literal::alias);

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
  auto ovl = overlay<ovl::ovlConfiguration>(copy, literal::configuration);

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
  auto ovl = overlay<ovl::ovlConfiguration>(copy, literal::configuration);

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
  auto id = overlay<ovl::ovlId>(copy, literal::id);

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
  auto ovl = overlay<ovl::ovlVersion>(copy, literal::version);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->setVersion(ovl));
  _document.writeJson();

  return self();
} catch (std::exception const& ex) {
  TRACE_(6, "setVersion() Exception:" << ex.what());
  ThrowOnFailure(CallUndo());

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::setConfigurableEntity(JSONDocument const& entity) try {
  TRACE_(9, "setConfigurableEntity() args  entity=<" << entity << ">");

  JSONDocument copy(entity);
  auto ovl = overlay<ovl::ovlConfigurableEntity>(copy, literal::configurable_entity);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->addConfigurableEntity(ovl));

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

void debug::enableJSONDocumentBuilder() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::JSONDocumentBuilder trace_enable");
}