#include "artdaq-database/Overlay/ovlDatabaseRecord.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace jsonliteral = artdaq::database::dataformats::literal;
namespace ovl = artdaq::database::overlay;
using namespace artdaq::database;
using namespace artdaq::database::overlay;
using namespace artdaq::database::result;
using result_t = artdaq::database::result_t;
using artdaq::database::sharedtypes::unwrap;

ovlDatabaseRecord::ovlDatabaseRecord(value_t& record)
    : ovlKeyValue(jsonliteral::database_record, record),
      _document{overlay<ovlDocument>(record, jsonliteral::document_node)},
      _comments{overlay<ovlComments, array_t>(record, jsonliteral::comments_node)},
      _origin{overlay<ovlOrigin>(record, jsonliteral::origin_node)},
      _version{overlay<ovlVersion, std::string>(record, jsonliteral::version_node)},
      _entities{overlay<ovlConfigurableEntities>(record, jsonliteral::configurable_entity_node)},
      _configurations{overlay<ovlConfigurations, array_t>(record, jsonliteral::configurations_node)},
      _changelog{overlay<ovlChangeLog, std::string>(record, jsonliteral::changelog)},
      _bookkeeping{overlay<ovlBookkeeping>(record, jsonliteral::bookkeeping)},
      _id{overlay<ovlId>(record, jsonliteral::id_node)},
      _aliases{overlay<ovlAliases>(record, jsonliteral::aliases_node)} {}

ovlDocument& ovlDatabaseRecord::document() { return *_document; }

result_t ovlDatabaseRecord::swap(ovlDocumentUPtr_t& document) {
  if (isReadonly()) return Failure(msg_IsReadonly);

  std::swap(_document, document);

  return Success();
}

ovlComments& ovlDatabaseRecord::comments() { return *_comments; }

result_t ovlDatabaseRecord::swap(ovlCommentsUPtr_t& comments) {
  confirm(comments);

  if (isReadonly()) return Failure(msg_IsReadonly);

  std::swap(_comments, comments);

  return Success();
}

ovlOrigin& ovlDatabaseRecord::origin() { return *_origin; }

result_t ovlDatabaseRecord::swap(ovlOriginUPtr_t& origin) {
  confirm(origin);

  if (isReadonly()) return Failure(msg_IsReadonly);

  std::swap(_origin, origin);

  return Success();
}

ovlVersion& ovlDatabaseRecord::version() { return *_version; }

result_t ovlDatabaseRecord::swap(ovlVersionUPtr_t& version) {
  confirm(version);

  if (isReadonly()) return Failure(msg_IsReadonly);

  return _version->ovlKeyValue::swap(version.get());
}

/*
ovlConfigurableEntity& ovlDatabaseRecord::configurableEntity() { return _entities; }

result_t ovlDatabaseRecord::swap(ovlConfigurableEntityUPtr_t& entity) {
  confirm(entity);

  if (isReadonly()) return Failure(msg_IsReadonly);

  std::swap(_entities, entity);

  return Success();
}
*/

ovlConfigurations& ovlDatabaseRecord::configurations() { return *_configurations; }

ovlBookkeeping& ovlDatabaseRecord::bookkeeping() { return *_bookkeeping; }

ovlId& ovlDatabaseRecord::id() { return *_id; }

result_t ovlDatabaseRecord::swap(ovlIdUPtr_t& id) {
  confirm(id);

  if (isReadonly()) return Failure(msg_IsReadonly);

  std::swap(_id, id);

  return Success();
}

std::string ovlDatabaseRecord::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{";
  oss << debrace(_id->to_string()) << ",\n";
  oss << debrace(_version->to_string()) << ",\n";
  oss << debrace(_origin->to_string()) << ",\n";
  oss << debrace(_aliases->to_string()) << "\n";
  oss << debrace(_bookkeeping->to_string()) << ",\n";
  oss << debrace(_changelog->to_string()) << ",\n";
  oss << debrace(_entities->to_string()) << ",\n";
  oss << debrace(_configurations->to_string()) << ",\n";
  oss << debrace(_comments->to_string()) << ",\n";
  oss << debrace(_document->to_string()) << "\n";
  oss << "}";

  return oss.str();
}

bool& ovlDatabaseRecord::isReadonly() { return _bookkeeping->isReadonly(); }
bool const& ovlDatabaseRecord::isReadonly() const { return _bookkeeping->isReadonly(); }

result_t ovlDatabaseRecord::markReadonly() {
  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  _bookkeeping->markReadonly(true);

  return Success();
}
result_t ovlDatabaseRecord::markDeleted() {
  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  _bookkeeping->markDeleted(true);

  return Success();
}

result_t ovlDatabaseRecord::addConfiguration(ovlConfigurationUPtr_t& configuration) {
  confirm(configuration);

  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  auto update = std::string("addConfiguration");

  auto result = _configurations->add(configuration);

  if (!result.first) return result;

  return _bookkeeping->postUpdate(update, configuration);
}

result_t ovlDatabaseRecord::removeConfiguration(ovlConfigurationUPtr_t& configuration) {
  confirm(configuration);

  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  auto update = std::string("removeConfiguration");

  auto result = _configurations->remove(configuration);

  if (!result.first) return result;

  return _bookkeeping->postUpdate(update, configuration);
}

result_t ovlDatabaseRecord::addAlias(ovlAliasUPtr_t& alias) {
  confirm(alias);

  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  auto update = std::string("addAlias");

  auto result = _aliases->add(alias);

  if (!result.first) return result;

  return _bookkeeping->postUpdate(update, alias);
}

result_t ovlDatabaseRecord::removeAlias(ovlAliasUPtr_t& alias) {
  confirm(alias);

  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  auto update = std::string("removeAlias");

  auto result = _aliases->remove(alias);

  if (!result.first) return result;

  return _bookkeeping->postUpdate(update, alias);
}

result_t ovlDatabaseRecord::setVersion(ovlVersionUPtr_t& version) {
  confirm(version);

  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  auto result = swap(version);

  if (!result.first) return result;

  auto update = std::string("setVersion");

  return _bookkeeping->postUpdate(update, _version);
}

result_t ovlDatabaseRecord::addConfigurableEntity(ovlConfigurableEntityUPtr_t& entity) {
  confirm(entity);

  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  auto update = std::string("addConfigurableEntity");

  auto result = _entities->add(entity);

  if (!result.first) return result;

  return _bookkeeping->postUpdate(update, entity);
}
result_t ovlDatabaseRecord::removeConfigurableEntity(ovlConfigurableEntityUPtr_t& entity) {
  confirm(entity);

  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  auto update = std::string("removeConfigurableEntity");

  auto result = _entities->remove(entity);

  if (!result.first) return result;

  return _bookkeeping->postUpdate(update, entity);
}

result_t ovlDatabaseRecord::operator==(ovlDatabaseRecord const& other) const {
  std::ostringstream oss;
  oss << "\nDatabase records disagree.";
  auto noerror_pos = oss.tellp();

  auto result = *_document == *other._document;

  if (!result.first) oss << result.second;

  result = *_comments == *other._comments;

  if (!result.first) oss << result.second;

  result = *_origin == *other._origin;

  if (!result.first) oss << result.second;

  result = *_version == *other._version;

  if (!result.first) oss << result.second;

  result = *_configurations == *other._configurations;

  if (!result.first) oss << result.second;

  result = *_changelog == *other._changelog;

  if (!result.first) oss << result.second;

  result = *_bookkeeping == *other._bookkeeping;

  if (!result.first) oss << result.second;

  result = *_id == *other._id;

  if (!result.first) oss << result.second;

  result = *_entities == *other._entities;

  if (!result.first) oss << result.second;

  if (oss.tellp() == noerror_pos) return Success();

  // oss << "\n  Debug info:";
  // oss << "\n  Self  value: " << to_string();
  // oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}
