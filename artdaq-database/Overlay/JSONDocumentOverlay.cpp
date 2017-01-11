#include "artdaq-database/Overlay/JSONDocumentOverlay.h"

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::json::array_t;
using artdaq::database::json::type_t;

namespace jsonliteral = artdaq::database::dataformats::literal;

namespace ovl = artdaq::database::overlay;

using namespace artdaq::database;
using namespace artdaq::database::overlay;

using namespace artdaq::database::result;
using result_t = artdaq::database::result_t;

using artdaq::database::sharedtypes::unwrap;

ovlKeyValue::ovlKeyValue(object_t::key_type const& key, value_t& value) : _key(key), _value(value) {}

std::string ovlKeyValue::to_string() const noexcept {
  auto retValue = std::string{};
  auto tmpAST = object_t{};

  tmpAST[_key] = _value;

  using artdaq::database::json::JsonWriter;

  if (JsonWriter().write(tmpAST, retValue))
    return retValue;
  else
    return msg_ConvertionError;
}

value_t& ovlKeyValue::value(object_t::key_type const& key) { return objectValue(key); }

array_t& ovlKeyValue::array_value() { return arrayValue(); }

object_t& ovlKeyValue::object_value() { return objectValue(); }

object_t::key_type& ovlKeyValue::key() { return _key; }

object_t::key_type const& ovlKeyValue::key() const { return _key; }

value_t& ovlKeyValue::value() { return _value; }

value_t const& ovlKeyValue::value() const { return _value; }

std::string& ovlKeyValue::string_value() { return objectStringValue(); }

std::string const& ovlKeyValue::string_value() const { return objectStringValue(); }

value_t& ovlKeyValue::objectValue(object_t::key_type const& key) { return unwrap(_value).value<object_t>(key); }

array_t& ovlKeyValue::arrayValue() { return unwrap(_value).value_as<array_t>(); }

object_t& ovlKeyValue::objectValue() { return unwrap(_value).value_as<object_t>(); }

std::string& ovlKeyValue::objectStringValue() { return unwrap(_value).value_as<std::string>(); }

ovlKeyValue const& ovlKeyValue::self() const { return *this; }

ovlKeyValue& ovlKeyValue::self() { return *this; }

std::string const& ovlKeyValue::objectStringValue() const { return unwrap(_value).value_as<std::string>(); }

ovlValueTimeStamp::ovlValueTimeStamp(object_t::key_type const& key, value_t& ts) : ovlKeyValue(key, ts) {}

std::string& ovlValueTimeStamp::timestamp() { return string_value(); }

std::string const& ovlValueTimeStamp::timestamp() const { return string_value(); }

std::string& ovlValueTimeStamp::timestamp(std::string const& ts) {
  confirm(!ts.empty());
  timestamp() = ts;

  return timestamp();
}

std::string ovlValueTimeStamp::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{" << quoted_(key()) << ":" << quoted_(timestamp()) << "}";

  return oss.str();
}

ovlData::ovlData(object_t::key_type const& key, value_t& data) : ovlKeyValue(key, data) {}

ovlMetadata::ovlMetadata(object_t::key_type const& key, value_t& metadata) : ovlKeyValue(key, metadata) {}

ovlDocument::ovlDocument(object_t::key_type const& key, value_t& document)
    : ovlKeyValue(key, document),
      _data{overlay<ovlData>(document, jsonliteral::data_node)},
      _metadata{overlay<ovlMetadata>(document, jsonliteral::metadata_node)} {}

ovlData& ovlDocument::data() { return *_data; }

ovlData const& ovlDocument::data() const { return *_data; }

void ovlDocument::swap(ovlDataUPtr_t& data) {
  confirm(data);

  std::swap(_data, data);
}

ovlMetadata& ovlDocument::metadata() { return *_metadata; }

ovlMetadata const& ovlDocument::metadata() const { return *_metadata; }

void ovlDocument::swap(ovlMetadataUPtr_t& metadata) {
  confirm(metadata);

  std::swap(_metadata, metadata);
}

void ovlDocument::make_empty() {
  value_t tmp1 = object_t{};
  auto data = overlay<ovlData>(tmp1, jsonliteral::data_node);
  std::swap(_data, data);

  value_t tmp2 = object_t{};
  auto metadata = overlay<ovlMetadata>(tmp2, jsonliteral::metadata_node);
  std::swap(_metadata, metadata);
}

std::string ovlDocument::to_string() const noexcept {
  std::ostringstream oss;
  oss << quoted_(jsonliteral::document_node) << ":{\n";
  oss << _data->to_string() << ",\n";
  oss << _metadata->to_string() << "}";

  return oss.str();
}

ovlComment::ovlComment(object_t::key_type const& key, value_t& comment) : ovlKeyValue(key, comment) {}

int& ovlComment::linenum() { return value_as<int>(jsonliteral::linenum); }

std::string& ovlComment::text() { return value_as<std::string>(jsonliteral::value); }

int const& ovlComment::linenum() const { return value_as<int>(jsonliteral::linenum); }

std::string const& ovlComment::text() const { return value_as<std::string>(jsonliteral::value); }

std::string ovlComment::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::linenum) << ":" << linenum() << ",";
  oss << quoted_(jsonliteral::value) << ":" << quoted_(text()) << "}";

  return oss.str();
}

ovlComments::ovlComments(object_t::key_type const& key, value_t& comments)
    : ovlKeyValue(key, comments), _comments{make_comments(ovlKeyValue::array_value())} {}

void ovlComments::wipe() { _comments = ovlComments::arrayComments_t{}; }

std::string ovlComments::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::comments_node) << ": [";

  for (auto const& comment : _comments) oss << "\n" << comment.to_string() << ",";

  if (!_comments.empty()) oss.seekp(-1, oss.cur);

  oss << "\n]}";

  return oss.str();
}

ovlComments::arrayComments_t ovlComments::make_comments(array_t& comments) {
  auto returnValue = ovlComments::arrayComments_t{};

  for (auto& comment : comments) returnValue.push_back({jsonliteral::comment, comment});

  return returnValue;
}

ovlOrigin::ovlOrigin(object_t::key_type const& key, value_t& origin)
    : ovlKeyValue(key, origin), _initOK(init(origin)), _timestamp(map_timestamp(origin)) {}

std::string& ovlOrigin::format() { return value_as<std::string>(jsonliteral::format); }

std::string const& ovlOrigin::format() const { return value_as<std::string>(jsonliteral::format); }

std::string& ovlOrigin::source() { return value_as<std::string>(jsonliteral::source); }

std::string const& ovlOrigin::source() const { return value_as<std::string>(jsonliteral::source); }

std::string ovlOrigin::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::origin_node) << ":{\n";
  oss << quoted_(jsonliteral::format) << ":" << quoted_(format()) << ",\n";
  oss << quoted_(jsonliteral::source) << ":" << quoted_(source()) << ",\n";
  oss << debrace(_timestamp.to_string()) << "\n";
  oss << "}}";

  return oss.str();
}

bool ovlOrigin::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::source) == 0) obj[jsonliteral::source] = std::string{"template"};

  if (obj.count(jsonliteral::format) == 0) obj[jsonliteral::format] = std::string{"json"};

  return true;
} catch (...) {
  confirm(false);
  throw;
}

ovlValueTimeStamp ovlOrigin::map_timestamp(value_t& parent) {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::timestamp) == 0) obj[jsonliteral::timestamp] = timestamp();

  confirm(obj.count(jsonliteral::timestamp) == 1);

  return ovlValueTimeStamp(jsonliteral::timestamp, obj.at(jsonliteral::timestamp));
}

ovlVersion::ovlVersion(object_t::key_type const& key, value_t& version)
    : ovlKeyValue(key, version), _initOK(init(version)) {}

bool ovlVersion::init(value_t& parent) try {
  confirm(type(parent) == type_t::VALUE);

  auto& version = string_value();

  if (version.empty()) version = std::string{jsonliteral::notprovided};

  return true;
} catch (...) {
  confirm(false);
  throw;
}

ovlConfigurableEntity::ovlConfigurableEntity(object_t::key_type const& key, value_t& entity)
    : ovlKeyValue(key, entity), _initOK(init(entity)) {}

bool ovlConfigurableEntity::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::name) == 0) obj[jsonliteral::name] = std::string{jsonliteral::notprovided};

  confirm(obj.count(jsonliteral::name) == 1);

  return true;
} catch (...) {
  confirm(false);
  throw;
}

ovlConfiguration::ovlConfiguration(object_t::key_type const& key, value_t& configuration)
    : ovlKeyValue(key, configuration), _timestamp(map_timestamp(configuration)) {}

std::string& ovlConfiguration::name() { return value_as<std::string>(jsonliteral::name); }

std::string const& ovlConfiguration::name() const { return value_as<std::string>(jsonliteral::name); }

std::string& ovlConfiguration::name(std::string const& name) {
  confirm(!name.empty());

  value() = name;

  return string_value();
}

std::string& ovlConfiguration::timestamp() { return _timestamp.timestamp(); }

std::string const& ovlConfiguration::timestamp() const { return _timestamp.timestamp(); }

std::string ovlConfiguration::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{";
  oss << quoted_(jsonliteral::name) << ":" << quoted_(name()) << ",";
  oss << debrace(_timestamp.to_string());
  oss << "}";

  return oss.str();
}

ovlValueTimeStamp ovlConfiguration::map_timestamp(value_t& value) {
  confirm(type(value) == type_t::OBJECT);

  auto& obj = object_value();
  confirm(obj.count(jsonliteral::assigned) == 1);

  return ovlValueTimeStamp(jsonliteral::assigned, obj.at(jsonliteral::assigned));
}

ovlAlias::ovlAlias(object_t::key_type const& key, value_t& alias)
    : ovlKeyValue(key, alias), _initOK(init(alias)), _assigned(map_assigned(alias)) {}

bool ovlAlias::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::assigned) == 0) obj[jsonliteral::assigned] = timestamp();

  confirm(obj.count(jsonliteral::assigned) == 1);
  confirm(obj.count(jsonliteral::name) == 1);

  return true;
} catch (...) {
  confirm(false);
  throw;
}
std::string& ovlAlias::name() { return value_as<std::string>(jsonliteral::name); }

std::string const& ovlAlias::name() const { return value_as<std::string>(jsonliteral::name); }

std::string& ovlAlias::name(std::string const& name) {
  confirm(!name.empty());

  value() = name;

  return string_value();
}

std::string& ovlAlias::assigned() { return _assigned.timestamp(); }

std::string ovlAlias::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{";
  oss << quoted_(jsonliteral::name) << ":" << quoted_(name()) << ",";
  oss << debrace(_assigned.to_string());
  oss << "}";

  return oss.str();
}

ovlValueTimeStamp ovlAlias::map_assigned(value_t& value) {
  confirm(type(value) == type_t::OBJECT);
  auto& obj = object_value();
  confirm(obj.count(jsonliteral::assigned) == 1);

  return ovlValueTimeStamp(jsonliteral::assigned, obj.at(jsonliteral::assigned));
}

ovlChangeLog::ovlChangeLog(object_t::key_type const& key, value_t& changelog)
    : ovlKeyValue(key, changelog), _initOK(init(changelog)) {}

bool ovlChangeLog::init(value_t& parent) try {
  confirm(type(parent) == type_t::VALUE);

  auto& changelog = string_value();

  if (changelog.empty()) changelog = std::string{jsonliteral::empty};

  return true;
} catch (...) {
  confirm(false);
  throw;
}

std::string& ovlChangeLog::value() { return value_as<std::string>(jsonliteral::changelog); }

std::string const& ovlChangeLog::value() const { return value_as<std::string>(jsonliteral::changelog); }

std::string& ovlChangeLog::value(std::string const& changelog) {
  value() = changelog;

  return value();
}

std::string& ovlChangeLog::append(std::string const& changelog) {
  value() += changelog;

  return value();
}

ovlConfigurations::ovlConfigurations(object_t::key_type const& key, value_t& configurations)
    : ovlKeyValue(key, configurations), _configurations(make_configurations(ovlKeyValue::array_value())) {}

std::string ovlConfigurations::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::configurations_node) << ": [";

  for (auto const& configuration : _configurations) oss << "\n" << configuration.to_string() << ",";

  if (!_configurations.empty()) oss.seekp(-1, oss.cur);

  oss << "\n]}";

  return oss.str();
}

void ovlConfigurations::wipe() { _configurations = ovlConfigurations::arrayConfigurations_t{}; }

ovlConfigurations::arrayConfigurations_t ovlConfigurations::make_configurations(array_t& configurations) {
  auto returnValue = ovlConfigurations::arrayConfigurations_t{};

  for (auto& configuration : configurations) returnValue.push_back({jsonliteral::configuration, configuration});

  return returnValue;
}

ovlId::ovlId(object_t::key_type const& key, value_t& oid) : ovlKeyValue(key, oid), _initOK(init(oid)) {}

bool ovlId::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::oid) == 0) obj[jsonliteral::oid] = generate_oid();

  confirm(obj.count(jsonliteral::oid) == 1);

  return true;
} catch (...) {
  confirm(false);
  throw;
}

std::string& ovlId::oid() { return value_as<std::string>(jsonliteral::oid); }

std::string& ovlId::oid(std::string const& id) {
  confirm(id.empty());

  oid() = id;
  return oid();
}

ovlUpdate::ovlUpdate(object_t::key_type const& key, value_t& update)
    : ovlKeyValue(key, update), _timestamp(map_timestamp(update)), _what(map_what(update)) {}

std::string& ovlUpdate::name() { return value_as<std::string>(jsonliteral::event); }

std::string const& ovlUpdate::name() const { return value_as<std::string>(jsonliteral::event); }

std::string& ovlUpdate::timestamp() { return _timestamp.timestamp(); }

std::string const& ovlUpdate::timestamp() const { return _timestamp.timestamp(); }

ovlKeyValue& ovlUpdate::what() { return _what; }

ovlKeyValue const& ovlUpdate::what() const { return _what; }

std::string ovlUpdate::to_string() const noexcept {
  std::ostringstream oss;

  oss << "{";
  oss << quoted_(jsonliteral::event) << ":" << quoted_(name()) << ",";
  oss << debrace(_timestamp.to_string()) << ",";
  oss << debrace(_what.to_string());
  oss << "}";

  return oss.str();
}

ovlValueTimeStamp ovlUpdate::map_timestamp(value_t& value) {
  confirm(type(value) == type_t::OBJECT);

  auto& obj = object_value();

  confirm(obj.count(jsonliteral::timestamp) == 1);

  return ovlValueTimeStamp(jsonliteral::timestamp, obj.at(jsonliteral::timestamp));
}

ovlKeyValue ovlUpdate::map_what(value_t& value) {
  confirm(type(value) == type_t::OBJECT);
  auto& obj = object_value();

  confirm(obj.count(jsonliteral::value) == 1);

  return ovlKeyValue(jsonliteral::value, obj.at(jsonliteral::value));
}

ovlBookkeeping::ovlBookkeeping(object_t::key_type const& key, value_t& bookkeeping)
    : ovlKeyValue(key, bookkeeping),
      _initOK(init(bookkeeping)),
      _updates(make_updates(bookkeeping)),
      _created(map_created(bookkeeping)) {}

bool& ovlBookkeeping::isReadonly() { return value_as<bool>(jsonliteral::isreadonly); }

bool const& ovlBookkeeping::isReadonly() const { return value_as<bool>(jsonliteral::isreadonly); }

bool& ovlBookkeeping::markReadonly(bool const& state) {
  value_as<bool>(jsonliteral::isreadonly) = state;

  return isReadonly();
}

bool& ovlBookkeeping::isDeleted() { return value_as<bool>(jsonliteral::isdeleted); }

bool const& ovlBookkeeping::isDeleted() const { return value_as<bool>(jsonliteral::isdeleted); }

bool& ovlBookkeeping::markDeleted(bool const& state) {
  value_as<bool>(jsonliteral::isdeleted) = state;

  return isDeleted();
}

std::string ovlBookkeeping::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::bookkeeping) << ": {";
  oss << quoted_(jsonliteral::isreadonly) << ":" << bool_(isReadonly()) << ",\n";
  oss << quoted_(jsonliteral::isdeleted) << ":" << bool_(isDeleted()) << ",\n";
  oss << debrace(_created.to_string()) << ",\n";
  oss << quoted_(jsonliteral::updates) << ": [";

  for (auto const& update : _updates) oss << "\n" << update.to_string() << ",";

  if (!_updates.empty()) oss.seekp(-1, oss.cur);

  oss << "\n]\n}\n}";

  return oss.str();
}

ovlBookkeeping::arrayBookkeeping_t ovlBookkeeping::make_updates(value_t& value) {
  confirm(type(value) == type_t::OBJECT);
  auto& obj = object_value();

  if (obj.count(jsonliteral::updates) == 0) obj[jsonliteral::updates] = array_t{};

  confirm(obj.count(jsonliteral::updates) == 1);

  auto& updates = unwrap(obj).value_as<array_t>(jsonliteral::updates);

  auto returnValue = ovlBookkeeping::arrayBookkeeping_t{};

  for (auto& update : updates) returnValue.push_back({jsonliteral::update, update});

  return returnValue;
}

ovlValueTimeStamp ovlBookkeeping::map_created(value_t& value) {
  confirm(type(value) == type_t::OBJECT);
  auto& obj = object_value();

  if (obj.count(jsonliteral::created) == 0) obj[jsonliteral::created] = timestamp();

  confirm(obj.count(jsonliteral::created) == 1);

  return ovlValueTimeStamp(jsonliteral::created, obj.at(jsonliteral::created));
}

bool ovlBookkeeping::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::isdeleted) == 0) obj[jsonliteral::isdeleted] = false;

  if (obj.count(jsonliteral::isreadonly) == 0) obj[jsonliteral::isreadonly] = false;

  return true;
} catch (...) {
  confirm(false);
  throw;
}

ovlDatabaseRecord::ovlDatabaseRecord(value_t& record)
    : ovlKeyValue(jsonliteral::database_record, record),
      _document{overlay<ovlDocument>(record, jsonliteral::document_node)},
      _comments{overlay<ovlComments, array_t>(record, jsonliteral::comments_node)},
      _origin{overlay<ovlOrigin>(record, jsonliteral::origin_node)},
      _version{overlay<ovlVersion, std::string>(record, jsonliteral::version_node)},
      _configurableEntity{overlay<ovlConfigurableEntity>(record, jsonliteral::configurable_entity_node)},
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

  std::swap(_version, version);

  return Success();
}

ovlConfigurableEntity& ovlDatabaseRecord::configurableEntity() { return *_configurableEntity; }

result_t ovlDatabaseRecord::swap(ovlConfigurableEntityUPtr_t& entity) {
  confirm(entity);

  if (isReadonly()) return Failure(msg_IsReadonly);

  std::swap(_configurableEntity, entity);

  return Success();
}

ovlConfigurations& ovlDatabaseRecord::configurations() { return *_configurations; }

result_t ovlDatabaseRecord::swap(ovlConfigurationsUPtr_t& configurations) {
  if (isReadonly()) return Failure(msg_IsReadonly);

  std::swap(_configurations, configurations);

  return Success();
}

ovlBookkeeping& ovlDatabaseRecord::bookkeeping() { return *_bookkeeping; }

result_t ovlDatabaseRecord::swap(ovlBookkeepingUPtr_t& bookkeeping) {
  confirm(bookkeeping);

  if (isReadonly()) return Failure(msg_IsReadonly);

  std::swap(_bookkeeping, bookkeeping);

  return Success();
}

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
  oss << debrace(_configurableEntity->to_string()) << ",\n";
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

result_t ovlConfigurations::add(ovlConfigurationUPtr_t& newConfiguration) {
  confirm(newConfiguration);

  for (auto& configuration : _configurations) {
    if (configuration.name() == newConfiguration->name()) {
      // configuration.timestamp().swap(newConfiguration->timestamp());
      return Success(msg_Ignored);
    }
  }

  auto& configurations = ovlKeyValue::array_value();
  // update AST
  configurations.push_back(newConfiguration->value());
  // reattach AST
  _configurations = make_configurations(configurations);

  return Success(msg_Added);
}

result_t ovlConfigurations::remove(ovlConfigurationUPtr_t& oldConfiguration) {
  confirm(oldConfiguration);

  if (_configurations.empty()) Success(msg_Ignored);

  auto& configurations = ovlKeyValue::array_value();

  auto oldCount = configurations.size();

  configurations.erase(std::remove_if(configurations.begin(), configurations.end(),
                                      [&oldConfiguration](value_t& configuration) -> bool {
                                        auto candidateConfiguration = std::make_unique<ovlConfiguration>(
                                            jsonliteral::configuration, configuration);
                                        return candidateConfiguration->name() == oldConfiguration->name();
                                      }),
                       configurations.end());

  if (oldCount == configurations.size()) return Failure(msg_Missing);

  confirm(oldCount == configurations.size() + 1);

  oldConfiguration->object_value()[jsonliteral::removed] = timestamp();

  _configurations = make_configurations(configurations);

  return Success(msg_Removed);
}

ovlAliases::ovlAliases(object_t::key_type const& key, value_t& aliases)
    : ovlKeyValue(key, aliases),
      _initOK(init(aliases)),
      _active(make_aliases(ovlKeyValue::value_as<array_t>(jsonliteral::active))),
      _history(make_aliases(ovlKeyValue::value_as<array_t>(jsonliteral::history))) {}

bool ovlAliases::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::active) == 0) obj[jsonliteral::active] = array_t{};

  if (obj.count(jsonliteral::history) == 0) obj[jsonliteral::history] = array_t{};

  return true;
} catch (...) {
  confirm(false);
  throw;
}

void ovlAliases::wipe() {
  _active = ovlAliases::arrayAliases_t{};
  _history = ovlAliases::arrayAliases_t{};
}

result_t ovlAliases::add(ovlAliasUPtr_t& newAlias) {
  confirm(newAlias);

  for (auto& alias : _active) {
    if (alias.name() == newAlias->name()) {
      // alias.timestamp().swap(newAlias->timestamp());
      return Success(msg_Ignored);
    }
  }

  auto& aliases = ovlKeyValue::value_as<array_t>(jsonliteral::active);
  // update AST
  aliases.push_back(newAlias->value());
  // reattach AST
  _active = make_aliases(aliases);

  return Success(msg_Added);
}

result_t ovlAliases::remove(ovlAliasUPtr_t& oldAlias) {
  confirm(oldAlias);

  if (_active.empty()) Success(msg_Ignored);

  auto& aliases = ovlKeyValue::value_as<array_t>(jsonliteral::active);

  auto oldCount = aliases.size();

  aliases.erase(std::remove_if(aliases.begin(), aliases.end(),
                               [&oldAlias](value_t& alias) -> bool {
                                 auto candidateAlias = std::make_unique<ovlAlias>(jsonliteral::alias, alias);
                                 return candidateAlias->name() == oldAlias->name();
                               }),
                aliases.end());

  if (oldCount == aliases.size()) return Failure(msg_Missing);

  confirm(oldCount - 1 == aliases.size());

  _active = make_aliases(aliases);

  auto& history = ovlKeyValue::value_as<array_t>(jsonliteral::history);

  oldCount = history.size();

  oldAlias->object_value()[jsonliteral::removed] = timestamp();

  history.push_back(oldAlias->value());

  confirm(oldCount + 1 == history.size());

  _history = make_aliases(history);

  return Success(msg_Removed);
}

std::string ovlAliases::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::aliases) << ": {";
  oss << quoted_(jsonliteral::active) << ": [";
  for (auto const& alias : _active) oss << "\n" << alias.to_string() << ",";
  if (!_active.empty()) oss.seekp(-1, oss.cur);
  oss << "\n],";
  oss << quoted_(jsonliteral::history) << ": [";
  for (auto const& alias : _history) oss << "\n" << alias.to_string() << ",";
  if (!_history.empty()) oss.seekp(-1, oss.cur);
  oss << "\n]\n}\n}";

  return oss.str();
}

ovlAliases::arrayAliases_t ovlAliases::make_aliases(array_t& aliases) {
  auto returnValue = ovlAliases::arrayAliases_t{};

  for (auto& alias : aliases) returnValue.push_back({jsonliteral::alias, alias});

  return returnValue;
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

  auto update = std::string("setVersion");

  auto result = swap(version);

  if (!result.first) return result;

  return _bookkeeping->postUpdate(update, version);
}

result_t ovlDatabaseRecord::addConfigurableEntity(ovlConfigurableEntityUPtr_t& entity) {
  confirm(entity);

  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  auto update = std::string("addConfigurableEntity");

  auto result = swap(entity);

  if (!result.first) return result;

  return _bookkeeping->postUpdate(update, entity);
}
result_t ovlDatabaseRecord::removeConfigurableEntity(ovlConfigurableEntityUPtr_t& entity) {
  confirm(entity);

  if (_bookkeeping->isReadonly()) return Failure(msg_IsReadonly);

  auto update = std::string("removeConfigurableEntity");

  auto result = swap(entity);

  if (!result.first) return result;

  return _bookkeeping->postUpdate(update, entity);
}

std::uint32_t artdaq::database::overlay::useCompareMask(std::uint32_t compareMask) {
  static std::uint32_t _compareMask = compareMask;

  return _compareMask;
}

result_t ovlKeyValue::operator==(ovlKeyValue const& other) const {
  if (_key != other._key) return {false, "Keys are different: self,other=" + _key + "," + other._key + "."};

  auto result = artdaq::database::json::operator==(_value, other._value);

  if (result.first) return result;

  using artdaq::database::json::print_visitor;

  std::ostringstream oss;
  oss << "\n  Values disagree.";
  oss << "\n  Key: " << quoted_(_key);
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}

result_t ovlValueTimeStamp::operator==(ovlValueTimeStamp const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_TIMESTAMPS) == DOCUMENT_COMPARE_MUTE_TIMESTAMPS)
             ? Success()
             : self() == other.self();
}

result_t ovlId::operator==(ovlId const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_OUIDS) == DOCUMENT_COMPARE_MUTE_OUIDS) ? Success()
                                                                                           : self() == other.self();
}

result_t ovlOrigin::operator==(ovlOrigin const& other) const {
  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_ORIGIN) == DOCUMENT_COMPARE_MUTE_ORIGIN) return Success();

  std::ostringstream oss;
  oss << "\nOrigin nodes disagree.";
  auto noerror_pos = oss.tellp();

  if (format() != other.format())
    oss << "\n  Formats are different: self,other=" << quoted_(format()) << "," << quoted_(other.format());

  if (source() != other.source())
    oss << "\n  Sources are different: self,other=" << quoted_(source()) << "," << quoted_(other.source());

  auto result = _timestamp == other._timestamp;

  if (!result.first)
    oss << "\n  Timestamps are different: self,other=" << quoted_(_timestamp.timestamp()) << ","
        << quoted_(other._timestamp.timestamp());

  if (oss.tellp() == noerror_pos) return Success();

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}

result_t ovlBookkeeping::operator==(ovlBookkeeping const& other) const {
  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_BOOKKEEPING) == DOCUMENT_COMPARE_MUTE_BOOKKEEPING) return Success();

  std::ostringstream oss;
  oss << "\nBookkeeping nodes disagree.";
  auto noerror_pos = oss.tellp();

  if (isDeleted() != other.isDeleted())
    oss << "\n  isdeleted flags are different: self,other=" << bool_(isDeleted()) << "," << bool_(other.isDeleted());

  if (isReadonly() != other.isReadonly())
    oss << "\n  isreadonly flags are different: self,other=" << bool_(isReadonly()) << "," << bool_(other.isReadonly());

  auto result = _created == other._created;

  if (!result.first)
    oss << "\n  Timestamps are different: self,other=" << quoted_(_created.timestamp()) << ","
        << quoted_(other._created.timestamp());

  if (oss.tellp() == noerror_pos && (useCompareMask() & DOCUMENT_COMPARE_MUTE_UPDATES) == DOCUMENT_COMPARE_MUTE_UPDATES)
    return Success();

  if (_updates.size() != other._updates.size())
    oss << "\n  Record update histories have different size: self,other=" << _updates.size() << ","
        << other._updates.size();

  if (oss.tellp() == noerror_pos && std::equal(_updates.cbegin(), _updates.end(), other._updates.cbegin(),
                                               [&oss](auto const& first, auto const& second) -> bool {
                                                 auto result = first == second;
                                                 if (result.first) return true;
                                                 oss << "\n  Record update histories are different: self,other="
                                                     << first.to_string() << "," << second.to_string();
                                                 return false;
                                               }))
    return Success();

  oss << "\n  Debug info:";
  oss << "\n  Self  value:\n" << to_string();
  oss << "\n  Other value:\n" << other.to_string();

  return Failure(oss);
}

result_t ovlAliases::operator==(ovlAliases const& other) const {
  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_ALIASES) == DOCUMENT_COMPARE_MUTE_ALIASES) return Success();

  std::ostringstream oss;
  oss << "\nAliases nodes disagree.";
  auto noerror_pos = oss.tellp();

  if (_active.size() != other._active.size())
    oss << "\n  Different active alias count: self,other=" << _active.size() << "," << other._active.size();

  if (oss.tellp() == noerror_pos && std::equal(_active.cbegin(), _active.end(), other._active.cbegin(),
                                               [&oss](auto const& first, auto const& second) -> bool {
                                                 auto result = first == second;
                                                 if (result.first) return true;
                                                 oss << "\n  Aliases are different: self,other=" << first.to_string()
                                                     << "," << second.to_string();
                                                 return false;
                                               }))
    return Success();

  oss << "\n  Debug info:";
  oss << "\n  Self  value:\n" << to_string();
  oss << "\n  Other value:\n" << other.to_string();

  return Failure(oss);
}

result_t ovlVersion::operator==(ovlVersion const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_VERSION) == DOCUMENT_COMPARE_MUTE_VERSION) ? Success()
                                                                                               : self() == other.self();
}

result_t ovlData::operator==(ovlData const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_DATA) == DOCUMENT_COMPARE_MUTE_DATA) ? Success()
                                                                                         : self() == other.self();
}

result_t ovlMetadata::operator==(ovlMetadata const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_METADATA) == DOCUMENT_COMPARE_MUTE_METADATA)
             ? Success()
             : self() == other.self();
}

result_t ovlChangeLog::operator==(ovlChangeLog const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_CHANGELOG) == DOCUMENT_COMPARE_MUTE_CHANGELOG)
             ? Success()
             : self() == other.self();
}

result_t ovlUpdate::operator==(ovlUpdate const& other) const {
  std::ostringstream oss;
  oss << "\nUpdate events disagree.";
  auto noerror_pos = oss.tellp();

  auto result = _timestamp == other._timestamp;

  if (!result.first)
    oss << "\n  Timestamps are different: self,other=" << quoted_(_timestamp.timestamp()) << ","
        << quoted_(other._timestamp.timestamp());

  if (name() != other.name())
    oss << "\n  Events are different: self,other=" << quoted_(name()) << "," << quoted_(other.name());

  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_TIMESTAMPS) == DOCUMENT_COMPARE_MUTE_TIMESTAMPS) {
    auto const& name = _what.value_as<std::string>(jsonliteral::name);
    auto const& otherName = other._what.value_as<std::string>(jsonliteral::name);

    if (name != otherName)
      oss << "\n  Event names are different: self,other=" << quoted_(name) << "," << quoted_(otherName);
  } else {
    result = _what == other._what;

    if (!result.first)
      oss << "\n  Event data are different: self,other=" << what().to_string() << "," << other.what().to_string();
  }

  if (oss.tellp() == noerror_pos) return Success();

  return Failure(oss);
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

  result = *_configurableEntity == *other._configurableEntity;

  if (!result.first) oss << result.second;

  if (oss.tellp() == noerror_pos) return Success();

  // oss << "\n  Debug info:";
  // oss << "\n  Self  value: " << to_string();
  // oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}

result_t ovlDocument::operator==(ovlDocument const& other) const {
  std::ostringstream oss;
  oss << "\nUser data disagree.";
  auto noerror_pos = oss.tellp();

  auto result = *_data == *other._data;

  if (!result.first) oss << "\n  Data are different:\n  " << result.second;

  result = *_metadata == *other._metadata;

  if (!result.first) oss << "\n  Metadata are different:\n  " << result.second;

  if (oss.tellp() == noerror_pos) return Success();

  //  oss << "\n  Debug info:";
  //  oss << "\n  Self  value: " << to_string();
  //  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}

result_t ovlComment::operator==(ovlComment const& other) const { return self() == other.self(); }

result_t ovlConfiguration::operator==(ovlConfiguration const& other) const {
  std::ostringstream oss;
  oss << "\nConfiguration records disagree.";
  auto noerror_pos = oss.tellp();

  if (name() != other.name())
    oss << "\n  Configuration names are different: self,other=" << quoted_(name()) << "," << quoted_(other.name());

  auto result = _timestamp == other._timestamp;

  if (!result.first) oss << "\n  Timestamps are different:\n  " << result.second;

  if (oss.tellp() == noerror_pos) return Success();

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}

result_t ovlAlias::operator==(ovlAlias const& other) const {
  std::ostringstream oss;
  oss << "\nAlias records disagree.";
  auto noerror_pos = oss.tellp();

  if (name() != other.name())
    oss << "\n  Alias names are different: self,other=" << quoted_(name()) << "," << quoted_(other.name());

  auto result = _assigned == other._assigned;

  if (!result.first) oss << "\n  Timestamps are different:\n  " << result.second;

  if (oss.tellp() == noerror_pos) return Success();

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}

result_t ovlConfigurableEntity::operator==(ovlConfigurableEntity const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_CONFIGENTITY) == DOCUMENT_COMPARE_MUTE_CONFIGENTITY)
             ? Success()
             : self() == other.self();
}

result_t ovlConfigurations::operator==(ovlConfigurations const& other) const {
  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_CONFIGURATIONS) == DOCUMENT_COMPARE_MUTE_CONFIGURATIONS)
    return Success();

  std::ostringstream oss;
  oss << "\nConfiguration lists disagree.";
  auto noerror_pos = oss.tellp();

  if (_configurations.size() != other._configurations.size())
    oss << "\n  Configuration lists have different size: self,other=" << _configurations.size() << ","
        << other._configurations.size();

  if (oss.tellp() == noerror_pos &&
      std::equal(_configurations.cbegin(), _configurations.end(), other._configurations.cbegin(),
                 [&oss](auto const& first, auto const& second) -> bool {
                   auto result = first == second;
                   if (result.first) return true;
                   oss << result.second;
                   return false;
                 }))
    return Success();
  else
    oss << "\n  Configuration lists are different";

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}

result_t ovlComments::operator==(ovlComments const& other) const {
  std::ostringstream oss;
  oss << "\nComment lists disagree.";
  auto noerror_pos = oss.tellp();

  if (_comments.size() != other._comments.size())
    oss << "\n  Comment lists have different size: self,other=" << _comments.size() << "," << other._comments.size();

  if (oss.tellp() == noerror_pos && std::equal(_comments.cbegin(), _comments.end(), other._comments.cbegin(),
                                               [&oss](auto const& first, auto const& second) -> bool {
                                                 auto result = first == second;
                                                 if (result.first) return true;
                                                 oss << result.second;
                                                 return false;
                                               }))
    return Success();
  else
    oss << "\n  Comment lists are different";

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}
