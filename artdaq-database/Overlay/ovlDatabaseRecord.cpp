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
      _document{overlay<ovlDocument>(record, jsonliteral::document)},
      _comments{overlay<ovlComments, array_t>(record, jsonliteral::comments)},
      _origin{overlay<ovlOrigin>(record, jsonliteral::origin)},
      _version{overlay<ovlVersion, std::string>(record, jsonliteral::version)},
      _entities{overlay<ovlEntities, array_t>(record, jsonliteral::entities)},
      _configurations{overlay<ovlConfigurations, array_t>(record, jsonliteral::configurations)},
      _changelog{overlay<ovlChangeLog, std::string>(record, jsonliteral::changelog)},
      _bookkeeping{overlay<ovlBookkeeping>(record, jsonliteral::bookkeeping)},
      _id{overlay<ovlId>(record, jsonliteral::id)},
      _collection{overlay<ovlCollection, std::string>(record, jsonliteral::collection)},
      _aliases{overlay<ovlAliases>(record, jsonliteral::aliases)},
      _runs{overlay<ovlRuns, array_t>(record, jsonliteral::runs)},
      _attachments{overlay<ovlAttachments, array_t>(record, jsonliteral::attachments)},
      _configurationtype{overlay<ovlConfigurationType, std::string>(record, jsonliteral::configurationtype)} {}

ovlDocument& ovlDatabaseRecord::document() { return *_document; }

result_t ovlDatabaseRecord::swap(ovlDocumentUPtr_t& document) {
  if (isReadonly()) return Failure(msg_IsReadonly);

  return _document->ovlKeyValue::swap(document.get());
}

ovlComments& ovlDatabaseRecord::comments() { return *_comments; }

result_t ovlDatabaseRecord::swap(ovlCommentsUPtr_t& comments) {
  confirm(comments);

  if (isReadonly()) return Failure(msg_IsReadonly);

  return _comments->ovlKeyValue::swap(comments.get());
}

ovlOrigin& ovlDatabaseRecord::origin() { return *_origin; }

result_t ovlDatabaseRecord::swap(ovlOriginUPtr_t& origin) {
  confirm(origin);

  if (isReadonly()) return Failure(msg_IsReadonly);

  return _origin->ovlKeyValue::swap(origin.get());
}

ovlVersion& ovlDatabaseRecord::version() { return *_version; }

result_t ovlDatabaseRecord::swap(ovlVersionUPtr_t& version) {
  confirm(version);

  if (isReadonly()) return Failure(msg_IsReadonly);

  return _version->ovlKeyValue::swap(version.get());
}

result_t ovlDatabaseRecord::swap(ovlConfigurationTypeUPtr_t& configtype) {
  confirm(configtype);

  if (isReadonly()) return Failure(msg_IsReadonly);

  return _configurationtype->ovlKeyValue::swap(configtype.get());
}

result_t ovlDatabaseRecord::swap(ovlCollectionUPtr_t& collection) {
  confirm(collection);

  if (isReadonly()) return Failure(msg_IsReadonly);

  return _collection->ovlKeyValue::swap(collection.get());
}

ovlEntities& ovlDatabaseRecord::entities() { return *_entities; }

ovlConfigurations& ovlDatabaseRecord::configurations() { return *_configurations; }

ovlBookkeeping& ovlDatabaseRecord::bookkeeping() { return *_bookkeeping; }

ovlId& ovlDatabaseRecord::id() { return *_id; }

result_t ovlDatabaseRecord::swap(ovlIdUPtr_t& id) {
  confirm(id);

  if (isReadonly()) return Failure(msg_IsReadonly);

  return _id->ovlKeyValue::swap(id.get());
}

std::string ovlDatabaseRecord::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{";
  oss << debrace(_id->to_string()) << ",\n";
  oss << debrace(_collection->to_string()) << ",\n";
  oss << debrace(_version->to_string()) << ",\n";
  oss << debrace(_origin->to_string()) << ",\n";
  oss << debrace(_aliases->to_string()) << ",\n";
  oss << debrace(_bookkeeping->to_string()) << ",\n";
  oss << debrace(_changelog->to_string()) << ",\n";
  oss << debrace(_entities->to_string()) << ",\n";
  oss << debrace(_runs->to_string()) << ",\n";
  oss << debrace(_attachments->to_string()) << ",\n";
  oss << debrace(_configurationtype->to_string()) << ",\n";
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

result_t ovlDatabaseRecord::setConfigurationType(ovlConfigurationTypeUPtr_t& configtype) {
  confirm(configtype);

  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  auto result = swap(configtype);

  if (!result.first) return result;

  auto update = std::string("setConfigurationType");

  return _bookkeeping->postUpdate(update, _configurationtype);
}

result_t ovlDatabaseRecord::setCollection(ovlCollectionUPtr_t& collection) {
  confirm(collection);

  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  auto result = swap(collection);

  if (!result.first) return result;

  auto update = std::string("setCollection");

  return _bookkeeping->postUpdate(update, _collection);
}

result_t ovlDatabaseRecord::addEntity(ovlEntityUPtr_t& entity) {
  confirm(entity);

  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  auto update = std::string("addEntity");

  auto result = _entities->add(entity);

  if (!result.first) return result;

  return _bookkeeping->postUpdate(update, entity);
}

result_t ovlDatabaseRecord::removeEntity(ovlEntityUPtr_t& entity) {
  confirm(entity);

  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  auto update = std::string("removeEntity");

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

  result = *_collection == *other._collection;

  if (!result.first) oss << result.second;

  result = *_configurationtype == *other._configurationtype;

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

  result = *_runs == *other._runs;

  if (!result.first) oss << result.second;

  result = *_aliases == *other._aliases;

  if (!result.first) oss << result.second;

  result = *_attachments == *other._attachments;

  if (!result.first) oss << result.second;

  if (oss.tellp() == noerror_pos) return Success();

  // oss << "\n  Debug info:";
  // oss << "\n  Self  value: " << to_string();
  // oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}
