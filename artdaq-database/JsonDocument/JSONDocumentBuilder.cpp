#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/JSONDocument_template.h"
#include "artdaq-database/JsonDocument/common.h"

#include <ctime>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSNU:DocuBldr_C"

namespace artdaq {
namespace database {
namespace jsonutils {

constexpr auto name = "\"name\"";
constexpr auto assigned = "\"assigned\"";
constexpr auto colon = ":";
constexpr auto quote = "\"";
constexpr auto comma = ",";

std::string quoted_(std::string const& text) { return "\"" + text + "\""; }
std::string operator"" _quoted(const char* text, std::size_t) { return "\"" + std::string(text) + "\""; }

bool useFakeTime(bool useFakeTime = false) {
  static bool _useFakeTime = useFakeTime;
  return _useFakeTime;
}

std::string timestamp() {
  std::time_t now = std::time(nullptr);
  auto result = std::string(std::asctime(std::localtime(&now)));
  result.pop_back();

  if (useFakeTime()) return "Mon Feb  8 14:00:30 2016";

  return result;
}

JSONDocumentBuilder::JSONDocumentBuilder() : _document(template__empty_document) {}

JSONDocumentBuilder::JSONDocumentBuilder(JSONDocument const& document) : _document(document) {}

JSONDocument JSONDocumentBuilder::_makeAlias(JSONDocument const& newAlias) const {
  std::ostringstream oss;

  oss << '{';
  oss << "name"_quoted << colon << quoted_(JSONDocument::value(newAlias));
  oss << '}';

  TRACE_(10, "_makeAlias() activeAlias<" << oss.str() << ">");

  return {oss.str()};
}

JSONDocument JSONDocumentBuilder::_makeObjectId(JSONDocument const& objectId) const {
  std::ostringstream oss;

  oss << '{';
  oss << "_oid"_quoted << colon << quoted_(JSONDocument::value(objectId));
  oss << '}';

  TRACE_(10, "_makeObjectId() objectId<" << oss.str() << ">");

  return {oss.str()};
}

JSONDocument JSONDocumentBuilder::_makeaddToGlobalConfig(JSONDocument const& config) const {
  std::ostringstream oss;
  oss << '{';
  oss << "name"_quoted << colon << quoted_(JSONDocument::value(config));
  oss << comma;
  oss << "assigned"_quoted << colon << quoted_(timestamp());
  oss << '}';

  TRACE_(10, "_makeaddToGlobalConfig() globalConfig<" << oss.str() << ">");

  return {oss.str()};
}

JSONDocument JSONDocumentBuilder::_makeActiveAlias(JSONDocument const& newAlias) const {
  std::ostringstream oss;
  oss << '{';
  oss << "name"_quoted << colon << quoted_(JSONDocument::value(newAlias));
  oss << comma;
  oss << "assigned"_quoted << colon << quoted_(timestamp());
  oss << '}';

  TRACE_(10, "_makeActiveAlias() activeAlias<" << oss.str() << ">");

  return {oss.str()};
}

JSONDocument JSONDocumentBuilder::_makeHistoryAlias(JSONDocument const& activeAlias) const {
  std::ostringstream oss;
  oss << '{';
  oss << "removed"_quoted << colon << quoted_(timestamp());
  oss << '}';

  auto historyAlias = JSONDocument{JSONDocument::value_at(activeAlias, 0)};

  historyAlias.insertChild({oss.str()}, "removed");

  TRACE_(10, "_makeHistoryAlias() historyAlias<" << historyAlias << ">");

  return historyAlias;
}

JSONDocument JSONDocumentBuilder::_makeReadonly() const {
  std::ostringstream oss;

  oss << '{';
  oss << "isreadonly"_quoted << colon << "true";
  oss << '}';

  return {oss.str()};
}

JSONDocument JSONDocumentBuilder::_makeDeleted() const {
  std::ostringstream oss;

  oss << '{';
  oss << "isdeleted"_quoted << colon << "true";
  oss << '}';

  return {oss.str()};
}

JSONDocument JSONDocumentBuilder::_wrap_as_payload(JSONDocument const& document) const {
  std::ostringstream oss;
  oss << '{';
  oss << "payload"_quoted << colon << document.to_string();
  oss << '}';

  return {oss.str()};
}

JSONDocument JSONDocumentBuilder::_bookkeeping_update_payload(std::string action = "update") const {
  std::ostringstream oss;
  oss << '{';
  oss << quoted_(action) << colon << quoted_(timestamp());
  oss << '}';

  return _wrap_as_payload({oss.str()});
}

using string_pair_t = std::pair<std::string, std::string>;

template <>
JSONDocument toJSONDocument<string_pair_t>(string_pair_t const& pair) {
  std::ostringstream oss;
  oss << '{';
  oss << quoted_(pair.first) << colon << quoted_(pair.second);
  oss << '}';

  return {oss.str()};
}

JSONDocumentBuilder& JSONDocumentBuilder::createFromData(JSONDocument const& document) {
  TRACE_(2, "createFrom() args  document=<" << document << ">");

  _createFromTemplate(JSONDocument(template__default_document));

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

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::addAlias(JSONDocument const& alias) {
  TRACE_(3, "addAlias() args  alias=<" << alias << ">");

  enforce();

  auto newAlias = _wrap_as_payload(_makeActiveAlias(alias));

  auto tmp_document = _document;
  tmp_document.appendChild(newAlias, literal::aliases_active);
  tmp_document.appendChild(_bookkeeping_update_payload(actions::addAlias), literal::bookkeeping_updates);

  _document = std::move(tmp_document);

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::removeAlias(JSONDocument const& alias) {
  TRACE_(4, "removeAlias() args  alias=<" << alias << ">");

  enforce();

  auto removeAlias = _wrap_as_payload(_makeAlias(alias));

  auto tmp_document = _document;

  // move alias to history
  auto removedAlias = tmp_document.removeChild(removeAlias, literal::aliases_active);
  tmp_document.appendChild(_wrap_as_payload(_makeHistoryAlias(removedAlias)), literal::aliases_history);

  // do bookkeeping
  tmp_document.appendChild(_bookkeeping_update_payload(actions::removeAlias), literal::bookkeeping_updates);

  _document = std::move(tmp_document);

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::addToGlobalConfig(JSONDocument const& globalconfig) {
  TRACE_(5, "addToGlobalConfig() args  globalconfig=<" << globalconfig << ">");

  enforce();

  auto newGlobalConfig = _wrap_as_payload(_makeaddToGlobalConfig(globalconfig));

  auto tmp_document = _document;
  tmp_document.appendChild(newGlobalConfig, literal::configurations);
  tmp_document.appendChild(_bookkeeping_update_payload(actions::addToGlobalConfig), literal::bookkeeping_updates);

  _document = std::move(tmp_document);

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::setObjectID(JSONDocument const& objectId) {
  TRACE_(8, "setObjectID() args  objectId=<" << objectId << ">");

  enforce();

  auto newObjectID = _wrap_as_payload(_makeObjectId(objectId));

  auto tmp_document = _document;
  tmp_document.insertChild(newObjectID, literal::id);

  _document = std::move(tmp_document);

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::setVersion(JSONDocument const& version) {
  TRACE_(6, "setVersion() args  version=<" << version << ">");

  enforce();

  auto newVersion = _wrap_as_payload(version);

  auto tmp_document = _document;
  tmp_document.replaceChild(newVersion, literal::version);
  tmp_document.appendChild(_bookkeeping_update_payload(actions::setVersion), literal::bookkeeping_updates);

  _document = std::move(tmp_document);

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::setConfigurableEntity(JSONDocument const& entity) {
  TRACE_(9, "setConfigurableEntity() args  entity=<" << entity << ">");

  enforce();

  auto newEntity = _wrap_as_payload(entity);

  auto tmp_document = _document;
  tmp_document.insertChild(newEntity, literal::configurable_entity);
  tmp_document.appendChild(_bookkeeping_update_payload(actions::setConfigurableEntity), literal::bookkeeping_updates);

  _document = std::move(tmp_document);

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::markReadonly() {
  TRACE_(6, "markReadonly()");

  enforce();

  auto newValue = _wrap_as_payload(_makeReadonly());

  auto tmp_document = _document;
  tmp_document.replaceChild(newValue, literal::bookkeeping_isreadonly);
  tmp_document.appendChild(_bookkeeping_update_payload(actions::markReadonly), literal::bookkeeping_updates);

  _document = std::move(tmp_document);

  return self();
}

JSONDocumentBuilder& JSONDocumentBuilder::markDeleted() {
  TRACE_(7, "markDeleted()");

  enforce();

  auto newValue = _wrap_as_payload(_makeDeleted());

  auto tmp_document = _document;
  tmp_document.replaceChild(newValue, literal::bookkeeping_isdeleted);
  tmp_document.appendChild(_bookkeeping_update_payload(actions::markDeleted), literal::bookkeeping_updates);

  newValue = _wrap_as_payload(_makeReadonly());
  tmp_document.replaceChild(newValue, literal::bookkeeping_isreadonly);
  tmp_document.appendChild(_bookkeeping_update_payload(actions::markReadonly), literal::bookkeeping_updates);

  _document = std::move(tmp_document);

  return self();
}

void JSONDocumentBuilder::enforce() const {
  if (!_document.isReadonly()) return;

  TRACE_(11, "enforce() Document is readonly.");
  throw notfound_exception("JSONDocumentBuilder") << "Document is readonly.";
}

namespace debug {
void enableJSONDocumentBuilder() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::JSONDocumentBuilder trace_enable");
}
}
}  // namespace jsonutils
}  // namespace database
}  // namespace artdaq
