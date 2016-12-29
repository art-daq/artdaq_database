#include "artdaq-database/Overlay/JSONDocumentOverlay.h"

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::json::array_t;
using artdaq::database::json::type_t;

namespace literal = artdaq::database::dataformats::literal;

using namespace artdaq::database;
using namespace artdaq::database::internals;

using artdaq::database::sharedtypes::unwrap;

/*
using artdaq::database::json::print_visitor;
using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;
*/

ovlKeyValue::ovlKeyValue(object_t::key_type const& key, value_t& value) : _key(key), _value(value) {}

std::string ovlKeyValue::to_string() const noexcept {
  auto retValue = std::string{};
  auto tmpAST = object_t{};
  tmpAST[_key] = _value;

  using artdaq::database::json::JsonWriter;

  if (JsonWriter().write(tmpAST, retValue))
    return retValue;
  else
    return "{ \"result\": \"conversion error\"}";
}

value_t& ovlKeyValue::value(object_t::key_type const& key) { return objectValue(key); }

array_t& ovlKeyValue::array_value() { return arrayValue(); }

object_t& ovlKeyValue::object_value() { return objectValue(); }

object_t::key_type& ovlKeyValue::key() { return _key; }

object_t::key_type const& ovlKeyValue::key() const { return _key; }

value_t& ovlKeyValue::value() { return _value; }

std::string& ovlKeyValue::stringValue() { return objectStringValue(); }

std::string const& ovlKeyValue::stringValue() const { return objectStringValue(); }

value_t& ovlKeyValue::objectValue(object_t::key_type const& key) { return unwrap(_value).value<object_t>(key); }

array_t& ovlKeyValue::arrayValue() { return unwrap(_value).value_as<array_t>(); }

object_t& ovlKeyValue::objectValue() { return unwrap(_value).value_as<object_t>(); }

std::string& ovlKeyValue::objectStringValue() { return unwrap(_value).value_as<std::string>(); }

ovlKeyValue const& ovlKeyValue::self() const { return *this; }
ovlKeyValue& ovlKeyValue::self() { return *this; }

std::string const& ovlKeyValue::objectStringValue() const { return unwrap(_value).value_as<std::string>(); }

ovlValueTimeStamp::ovlValueTimeStamp(object_t::key_type const& key, value_t& ts) : ovlKeyValue(key, ts) {}

std::string& ovlValueTimeStamp::timestamp() { return stringValue(); }

std::string const& ovlValueTimeStamp::timestamp() const { return stringValue(); }

std::string& ovlValueTimeStamp::timestamp(std::string const& ts) {
  assert(ts.empty());
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
      _data{overlay<ovlData>(document, literal::data_node)},
      _metadata{overlay<ovlMetadata>(document, literal::metadata_node)} {}

ovlData& ovlDocument::data() { return *_data; }

void ovlDocument::swap_data(std::unique_ptr<ovlData>& data) { std::swap(_data, data); }

ovlMetadata& ovlDocument::metadata() { return *_metadata; }

void ovlDocument::swap_metadata(std::unique_ptr<ovlMetadata>& metadata) { std::swap(_metadata, metadata); }

void ovlDocument::make_empty() {
  value_t tmp1 = object_t{};
  auto data = overlay<ovlData>(tmp1, literal::data_node);
  std::swap(_data, data);

  value_t tmp2 = object_t{};
  auto metadata = overlay<ovlMetadata>(tmp2, literal::metadata_node);
  std::swap(_metadata, metadata);
}

std::string ovlDocument::to_string() const noexcept {
  std::ostringstream oss;
  oss << quoted_(literal::document_node) << ":{\n";
  oss << _data->to_string() << ",\n";
  oss << _metadata->to_string() << "}";

  return oss.str();
}

ovlComment::ovlComment(object_t::key_type const& key, value_t& comment, array_t& parent)
    : ovlKeyValue(key, comment), _parent(parent) {}

int& ovlComment::linenum() { return value_as<int>(literal::linenum); }

std::string& ovlComment::text() { return value_as<std::string>(literal::value); }

int const& ovlComment::linenum() const { return value_as<int>(literal::linenum); }

std::string const& ovlComment::text() const { return value_as<std::string>(literal::value); }

std::string ovlComment::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{" << quoted_(literal::linenum) << ":" << linenum() << ",";
  oss << quoted_(literal::value) << ":" << quoted_(text()) << "}";

  return oss.str();
}

ovlComments::ovlComments(object_t::key_type const& key, value_t& comments)
    : ovlKeyValue(key, comments), _comments{make_comments(ovlKeyValue::array_value())} {}

void ovlComments::wipe() { _comments = ovlComments::arrayComments_t{}; }

std::string ovlComments::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{" << quoted_(literal::comments_node) << ": [";

  for (auto const& comment : _comments) oss << "\n" << comment.to_string() << ",";

  if (!_comments.empty()) oss.seekp(-1, oss.cur);

  oss << "\n]}";

  return oss.str();
}

ovlComments::arrayComments_t ovlComments::make_comments(array_t& comments) {
  auto returnValue = ovlComments::arrayComments_t{};

  for (auto& comment : comments) returnValue.push_back({literal::comment, comment, comments});

  return returnValue;
}

ovlOrigin::ovlOrigin(object_t::key_type const& key, value_t& origin)
    : ovlKeyValue(key, origin), _timestamp(map_timestamp(origin)) {}

std::string& ovlOrigin::format() { return value_as<std::string>(literal::format); }

std::string const& ovlOrigin::format() const { return value_as<std::string>(literal::format); }

std::string& ovlOrigin::source() { return value_as<std::string>(literal::source); }

std::string const& ovlOrigin::source() const { return value_as<std::string>(literal::source); }

std::string ovlOrigin::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{" << quoted_(literal::origin_node) << ":{\n";
  oss << quoted_(literal::format) << ":" << quoted_(format()) << ",\n";
  oss << quoted_(literal::source) << ":" << quoted_(source()) << ",\n";
  oss << debrace(_timestamp.to_string()) << "\n";
  oss << "}}";

  return oss.str();
}

ovlValueTimeStamp ovlOrigin::map_timestamp(value_t& value) {
  assert(type(value) == type_t::OBJECT);
  auto& obj = object_value();
  assert(obj.count(literal::timestamp) == 1);

  return ovlValueTimeStamp(literal::timestamp, obj.at(literal::timestamp));
}

ovlVersion::ovlVersion(object_t::key_type const& key, value_t& version) : ovlKeyValue(key, version) {}

ovlConfigurableEntity::ovlConfigurableEntity(object_t::key_type const& key, value_t& entity)
    : ovlKeyValue(key, entity) {}

ovlConfiguration::ovlConfiguration(object_t::key_type const& key, value_t& configuration, array_t& configurations)
    : ovlKeyValue(key, configuration), _parent(configurations), _timestamp(map_timestamp(configuration)) {}

std::string& ovlConfiguration::name() { return value_as<std::string>(literal::name); }

std::string const& ovlConfiguration::name() const { return value_as<std::string>(literal::name); }

std::string& ovlConfiguration::name(std::string const& name) {
  assert(!name.empty());

  value() = name;
  return stringValue();
}

std::string& ovlConfiguration::timestamp() { return _timestamp.timestamp(); }

std::string ovlConfiguration::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{";
  oss << quoted_(literal::name) << ":" << quoted_(name()) << ",";
  oss << debrace(_timestamp.to_string());
  oss << "}";

  return oss.str();
}

ovlValueTimeStamp ovlConfiguration::map_timestamp(value_t& value) {
  assert(type(value) == type_t::OBJECT);
  auto& obj = object_value();
  assert(obj.count(literal::assigned) == 1);

  return ovlValueTimeStamp(literal::assigned, obj.at(literal::assigned));
}

ovlChangeLog::ovlChangeLog(object_t::key_type const& key, value_t& changelog) : ovlKeyValue(key, changelog) {}

std::string& ovlChangeLog::value() { return value_as<std::string>(literal::changelog); }

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
  oss << "{" << quoted_(literal::configurations_node) << ": [";

  for (auto const& configuration : _configurations) oss << "\n" << configuration.to_string() << ",";

  if (!_configurations.empty()) oss.seekp(-1, oss.cur);

  oss << "\n]}";

  return oss.str();
}

void ovlConfigurations::wipe() { _configurations = ovlConfigurations::arrayConfigurations_t{}; }
// bool exists(std::string const& name) {}
// void add(std::string const& name) {}
// void remove(std::string name) {}

ovlConfigurations::arrayConfigurations_t ovlConfigurations::make_configurations(array_t& configurations) {
  auto returnValue = ovlConfigurations::arrayConfigurations_t{};

  for (auto& configuration : configurations)
    returnValue.push_back({literal::configuration, configuration, configurations});

  return returnValue;
}

ovlId::ovlId(object_t::key_type const& key, value_t& oid) : ovlKeyValue(key, oid) {}

std::string& ovlId::oid() { return value_as<std::string>(literal::oid); }

std::string& ovlId::oid(std::string const& id) {
  assert(id.empty());
  oid() = id;
  return oid();
}

ovlUpdate::ovlUpdate(object_t::key_type const& key, value_t& update, array_t& parent)
    : ovlKeyValue(key, update), _parent(parent), _operation(map_operation(update)) {}

std::string& ovlUpdate::opration() { return _operation.key(); }

std::string& ovlUpdate::timestamp() { return _operation.timestamp(); }

std::string ovlUpdate::to_string() const noexcept {
  std::ostringstream oss;
  oss << _operation.to_string();

  return oss.str();
}

ovlValueTimeStamp ovlUpdate::map_operation(value_t& value) {
  assert(type(value) == type_t::OBJECT);
  auto& obj = object_value();
  assert(obj.size() == 1);
  return ovlValueTimeStamp(obj.begin()->key, obj.begin()->value);
}

ovlBookkeeping::ovlBookkeeping(object_t::key_type const& key, value_t& bookkeeping)
    : ovlKeyValue(key, bookkeeping),
      _updates(make_updates(ovlKeyValue::value_as<array_t>(literal::updates))),
      _created(map_created(bookkeeping)) {}

bool& ovlBookkeeping::isReadonly() { return value_as<bool>(literal::isreadonly); }

bool const& ovlBookkeeping::isReadonly() const { return value_as<bool>(literal::isreadonly); }

bool& ovlBookkeeping::markReadonly(bool const& state) {
  value_as<bool>(literal::isreadonly) = state;
  return isReadonly();
}

bool& ovlBookkeeping::isDeleted() { return value_as<bool>(literal::isdeleted); }

bool const& ovlBookkeeping::isDeleted() const { return value_as<bool>(literal::isdeleted); }

bool& ovlBookkeeping::markDeleted(bool const& state) {
  value_as<bool>(literal::isdeleted) = state;
  return isDeleted();
}

std::string ovlBookkeeping::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{" << quoted_(literal::bookkeeping) << ": {";
  oss << quoted_(literal::isreadonly) << ":" << (isReadonly() ? "true" : "false") << ",\n";
  oss << quoted_(literal::isdeleted) << ":" << (isDeleted() ? "true" : "false") << ",\n";
  oss << debrace(_created.to_string()) << ",\n";
  oss << quoted_(literal::updates) << ": [";

  for (auto const& update : _updates) oss << "\n" << update.to_string() << ",";

  if (!_updates.empty()) oss.seekp(-1, oss.cur);

  oss << "\n]\n}\n}";

  return oss.str();
}

ovlBookkeeping::arrayBookkeeping_t ovlBookkeeping::make_updates(array_t& updates) {
  auto returnValue = ovlBookkeeping::arrayBookkeeping_t{};

  for (auto& update : updates) returnValue.push_back({literal::update, update, updates});

  return returnValue;
}

ovlValueTimeStamp ovlBookkeeping::map_created(value_t& value) {
  assert(type(value) == type_t::OBJECT);
  auto& obj = object_value();
  assert(obj.count(literal::created) == 1);

  return ovlValueTimeStamp(literal::created, obj.at(literal::created));
}

ovlDatabaseRecord::ovlDatabaseRecord(value_t& record)
    : ovlKeyValue(literal::database_record, record),
      _document{overlay<ovlDocument>(record, literal::document_node)},
      _comments{overlay<ovlComments, array_t>(record, literal::comments_node)},
      _origin{overlay<ovlOrigin>(record, literal::origin_node)},
      _version{overlay<ovlVersion>(record, literal::version_node)},
      _configurableEntity{overlay<ovlConfigurableEntity>(record, literal::configurable_entity_node)},
      _configurations{overlay<ovlConfigurations, array_t>(record, literal::configurations_node)},
      _changelog{overlay<ovlChangeLog, std::string>(record, literal::changelog)},
      _bookkeeping{overlay<ovlBookkeeping>(record, literal::bookkeeping)},
      _id{overlay<ovlId>(record, literal::id_node)} {}

ovlDocument& ovlDatabaseRecord::document() { return *_document; }

void ovlDatabaseRecord::swap_document(std::unique_ptr<ovlDocument>& document) { std::swap(_document, document); }

ovlComments& ovlDatabaseRecord::comments() { return *_comments; }

void ovlDatabaseRecord::swap_comments(std::unique_ptr<ovlComments>& comments) { std::swap(_comments, comments); }

ovlOrigin& ovlDatabaseRecord::origin() { return *_origin; }

void ovlDatabaseRecord::swap_origin(std::unique_ptr<ovlOrigin>& origin) { std::swap(_origin, origin); }

ovlVersion& ovlDatabaseRecord::version() { return *_version; }

void ovlDatabaseRecord::swap_version(std::unique_ptr<ovlVersion>& version) { std::swap(_version, version); }

ovlConfigurableEntity& ovlDatabaseRecord::configurableEntity() { return *_configurableEntity; }

void ovlDatabaseRecord::swap_configurableEntity(std::unique_ptr<ovlConfigurableEntity>& configurableEntity) {
  std::swap(_configurableEntity, configurableEntity);
}

ovlConfigurations& ovlDatabaseRecord::configurations() { return *_configurations; }

void ovlDatabaseRecord::swap_configurations(std::unique_ptr<ovlConfigurations>& configurations) {
  std::swap(_configurations, configurations);
}

ovlBookkeeping& ovlDatabaseRecord::bookkeeping() { return *_bookkeeping; }

void ovlDatabaseRecord::swap_bookkeeping(std::unique_ptr<ovlBookkeeping>& bookkeeping) {
  std::swap(_bookkeeping, bookkeeping);
}

ovlId& ovlDatabaseRecord::id() { return *_id; }

void ovlDatabaseRecord::swap_id(std::unique_ptr<ovlId>& id) { std::swap(_id, id); }

std::string ovlDatabaseRecord::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{";
  oss << debrace(_id->to_string()) << ",\n";
  oss << debrace(_version->to_string()) << ",\n";
  oss << debrace(_origin->to_string()) << ",\n";
  oss << debrace(_bookkeeping->to_string()) << ",\n";
  oss << debrace(_changelog->to_string()) << ",\n";
  oss << debrace(_configurableEntity->to_string()) << ",\n";
  oss << debrace(_configurations->to_string()) << ",\n";
  oss << debrace(_comments->to_string()) << ",\n";
  oss << debrace(_document->to_string()) << "\n";
  oss << "}";

  return oss.str();
}

std::string artdaq::database::quoted_(std::string const& text) { return "\"" + text + "\""; }

std::string artdaq::database::operator"" _quoted(const char* text, std::size_t) {
  return "\"" + std::string(text) + "\"";
}

std::string artdaq::database::debrace(std::string s) {
  if (s[0] == '{' && s[s.length() - 1] == '}')
    return s.substr(1, s.length() - 2);
  else
    return s;
}

std::uint32_t artdaq::database::internals::useCompareMask(std::uint32_t compareMask) {
  static std::uint32_t _compareMask = compareMask;
  return _compareMask;
}

constexpr auto noerror = "Success";

std::pair<bool, std::string> ovlKeyValue::operator==(ovlKeyValue const& other) const {
  if (_key != other._key) return {false, "Keys are different: self,other=" + _key + "," + other._key + "."};

  auto result = artdaq::database::json::operator==(_value, other._value);

  if (result.first) return result;

  using artdaq::database::json::print_visitor;

  std::ostringstream oss;
  oss << "\n  Values disagree.";
  oss << "\n  Key: " << quoted_(_key);
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return {false, oss.str()};
}

std::pair<bool, std::string> ovlValueTimeStamp::operator==(ovlValueTimeStamp const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_TIMESTAMPS)==DOCUMENT_COMPARE_MUTE_TIMESTAMPS) ? std::make_pair(true, noerror) : self() == other.self();
}

std::pair<bool, std::string> ovlId::operator==(ovlId const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_OUIDS)==DOCUMENT_COMPARE_MUTE_OUIDS) ? std::make_pair(true, noerror) : self() == other.self();
}

std::pair<bool, std::string> ovlOrigin::operator==(ovlOrigin const& other) const {
  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_ORIGIN)==DOCUMENT_COMPARE_MUTE_ORIGIN) return {true, noerror};

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

  if (oss.tellp() == noerror_pos) return {true, noerror};

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return {false, oss.str()};
}

std::pair<bool, std::string> ovlBookkeeping::operator==(ovlBookkeeping const& other) const {
  if ( (useCompareMask() & DOCUMENT_COMPARE_MUTE_BOOKKEEPING)==DOCUMENT_COMPARE_MUTE_BOOKKEEPING) return {true, noerror};

  std::ostringstream oss;
  oss << "\nBookkeeping nodes disagree.";
  auto noerror_pos = oss.tellp();

  if (isDeleted() != other.isDeleted())
    oss << "\n  isdeleted flags are different: self,other=" << (isDeleted() ? "true" : "false") << ","
        << (other.isDeleted() ? "true" : "false");

  if (isReadonly() != other.isReadonly())
    oss << "\n  isreadonly flags are different: self,other=" << (isReadonly() ? "true" : "false") << ","
        << (other.isReadonly() ? "true" : "false");

  auto result = _created == other._created;

  if (!result.first)
    oss << "\n  Timestamps are different: self,other=" << quoted_(_created.timestamp()) << ","
        << quoted_(other._created.timestamp());

  if (oss.tellp() == noerror_pos && (useCompareMask() & DOCUMENT_COMPARE_MUTE_UPDATES)==DOCUMENT_COMPARE_MUTE_UPDATES) return {true, noerror};

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
    return {true, noerror};

  oss << "\n  Debug info:";
  oss << "\n  Self  value:\n" << to_string();
  oss << "\n  Other value:\n" << other.to_string();

  return {false, oss.str()};
}

std::pair<bool, std::string> ovlVersion::operator==(ovlVersion const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_VERSION)==DOCUMENT_COMPARE_MUTE_VERSION) ? std::make_pair(true, noerror) : self() == other.self();
}

std::pair<bool, std::string> ovlData::operator==(ovlData const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_DATA)==DOCUMENT_COMPARE_MUTE_DATA) ? std::make_pair(true, noerror) : self() == other.self();
}

std::pair<bool, std::string> ovlMetadata::operator==(ovlMetadata const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_METADATA)==DOCUMENT_COMPARE_MUTE_METADATA) ? std::make_pair(true, noerror) : self() == other.self();
}

std::pair<bool, std::string> ovlChangeLog::operator==(ovlChangeLog const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_CHANGELOG)==DOCUMENT_COMPARE_MUTE_CHANGELOG) ? std::make_pair(true, noerror) : self() == other.self();
}

std::pair<bool, std::string> ovlUpdate::operator==(ovlUpdate const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_TIMESTAMPS)==DOCUMENT_COMPARE_MUTE_TIMESTAMPS) ? _operation == other._operation
                                                               : self() == other.self();
}

std::pair<bool, std::string> ovlDatabaseRecord::operator==(ovlDatabaseRecord const& other) const {
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

  if (oss.tellp() == noerror_pos) return {true, noerror};

  // oss << "\n  Debug info:";
  // oss << "\n  Self  value: " << to_string();
  // oss << "\n  Other value: " << other.to_string();

  return {false, oss.str()};
}

std::pair<bool, std::string> ovlDocument::operator==(ovlDocument const& other) const {
  std::ostringstream oss;
  oss << "\nUser data disagree.";
  auto noerror_pos = oss.tellp();

  auto result = *_data == *other._data;

  if (!result.first) oss << "\n  Data are different:\n  " << result.second;

  result = *_metadata == *other._metadata;

  if (!result.first) oss << "\n  Metadata are different:\n  " << result.second;

  if (oss.tellp() == noerror_pos) return {true, noerror};

//  oss << "\n  Debug info:";
//  oss << "\n  Self  value: " << to_string();
//  oss << "\n  Other value: " << other.to_string();

  return {false, oss.str()};
}

std::pair<bool, std::string> ovlComment::operator==(ovlComment const& other) const { return self() == other.self(); }

std::pair<bool, std::string> ovlConfiguration::operator==(ovlConfiguration const& other) const {
  std::ostringstream oss;
  oss << "\nConfiguration records disagree.";
  auto noerror_pos = oss.tellp();

  if (name() != other.name())
    oss << "\n  Configuration names are different: self,other=" << quoted_(name()) << "," << quoted_(other.name());

  auto result = _timestamp == other._timestamp;

  if (!result.first) oss << "\n  Timestamps are different:\n  " << result.second;

  if (oss.tellp() == noerror_pos) return {true, noerror};

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return {false, oss.str()};
}

std::pair<bool, std::string> ovlConfigurableEntity::operator==(ovlConfigurableEntity const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_CONFIGENTITY)==DOCUMENT_COMPARE_MUTE_CONFIGENTITY) ? std::make_pair(true, noerror)
                                                                 : self() == other.self();
}

std::pair<bool, std::string> ovlConfigurations::operator==(ovlConfigurations const& other) const {
  
  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_CONFIGURATIONS)==DOCUMENT_COMPARE_MUTE_CONFIGURATIONS) return std::make_pair(true, noerror);

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
    return {true, noerror};
  else
    oss << "\n  Configuration lists are different";

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return {false, oss.str()};
}

std::pair<bool, std::string> ovlComments::operator==(ovlComments const& other) const {
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
    return {true, noerror};
  else
    oss << "\n  Comment lists are different";

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return {false, oss.str()};
}
