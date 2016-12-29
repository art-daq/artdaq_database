#ifndef _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_OVERLAY_H_
#define _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_OVERLAY_H_

#include "artdaq-database/DataFormats/Json/json_common.h"
//#include "artdaq-database/DataFormats/common/shared_literals.h"
#include "artdaq-database/JsonDocument/common.h"

namespace artdaq {
namespace database {
namespace internals {

namespace literal = artdaq::database::dataformats::literal;

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::json::array_t;
using artdaq::database::json::type_t;

using artdaq::database::sharedtypes::unwrap;
using artdaq::database::json::JsonReader;

using artdaq::database::json::JsonWriter;

template <typename OVL, typename T = object_t>
static std::unique_ptr<OVL> overlay(value_t& parent, object_t::key_type const& self_key) {
  assert(!self_key.empty());
  assert(type(parent) == type_t::OBJECT);

  if (self_key.empty()) throw std::runtime_error("Errror: self_key is empty");

  if (type(parent) != type_t::OBJECT) throw std::runtime_error("Errror: parent is not a type_t::OBJECT type");

  return std::make_unique<OVL>(unwrap(parent).value<object_t, T>(self_key));
}

class ovlKeyValue {
 public:
  ovlKeyValue(object_t::key_type const& key, value_t& value) : _key(key), _value(value) {}
  ovlKeyValue(ovlKeyValue&&) = default;
  virtual ~ovlKeyValue() = default;

  virtual std::string to_string() const noexcept {
    auto retValue = std::string{};
    auto tmpAST = object_t{};
    tmpAST[_key] = _value;

    if (JsonWriter().write(tmpAST, retValue))
      return retValue;
    else
      return "{ \"result\": \"conversion error\"}";
  }

  value_t& value(object_t::key_type const& key) { return objectValue(key); }

  template <typename T>
  T& value_as(object_t::key_type const& key) {
    return objectValue<T>(key);
  }

 private:
  value_t& objectValue(object_t::key_type const& key) { return unwrap(_value).value<object_t>(key); }

  template <typename T>
  T& objectValue(object_t::key_type const& key) {
    return unwrap(_value).value_as<T>(key);
  }

  object_t::key_type _key;
  value_t& _value;
};

class ovlData final : public ovlKeyValue {
 public:
  ovlData(object_t::key_type const& key, value_t& data) : ovlKeyValue(key, data) {}
  ovlData(ovlData&&) = default;
  ~ovlData() = default;
};

class ovlMetadata final : public ovlKeyValue {
 public:
  ovlMetadata(object_t::key_type const& key, value_t& metadata) : ovlKeyValue(key, metadata) {}
  ovlMetadata(ovlMetadata&&) = default;
  ~ovlMetadata() = default;
};
class ovlDocument final : public ovlKeyValue {
 public:
  ovlDocument(object_t::key_type const& key, value_t& document)
      : ovlKeyValue(key, document),
        _data(overlay<ovlData>(document, literal::data_node)),
        _metadata(overlay<ovlMetadata>(document, literal::metadata_node)) {}
  ovlDocument(ovlDocument&&) = default;
  ~ovlDocument() = default;

  ovlData& data() { return *_data; }
  void swap_data(std::unique_ptr<ovlData>& data) { std::swap(_data, data); }

  ovlMetadata& metadata() { return *_metadata; }
  void swap_metadata(std::unique_ptr<ovlMetadata>& metadata) { std::swap(_metadata, metadata); }

  void make_empty() {
    value_t tmp1 = object_t{};
    auto data = overlay<ovlData>(tmp1, literal::data_node);
    std::swap(_data, data);

    value_t tmp2 = object_t{};
    auto metadata = overlay<ovlMetadata>(tmp2, literal::metadata_node);
    std::swap(_metadata, metadata);
  }

 private:
  std::unique_ptr<ovlData> _data;
  std::unique_ptr<ovlMetadata> _metadata;
};

class ovlComment final : public ovlKeyValue {
 public:
  ovlComment(object_t::key_type const& key, value_t& comment) : ovlKeyValue(key, comment) {}
  ovlComment(ovlComment&&) = default;
  ~ovlComment() = default;
  int& linenum() { return value_as<int>(literal::linenum); }
  std::string& value() { return value_as<std::string>(literal::value); }
};

class ovlComments final : public ovlKeyValue {
 public:
  ovlComments(object_t::key_type const& key, value_t& comments) : ovlKeyValue(key, comments) {}

  void wipe() { _comments = std::list<ovlComment>{}; }

 private:
  std::list<ovlComment> _comments;
};

class ovlOrigin final : public ovlKeyValue {
 public:
  ovlOrigin(object_t::key_type const& key, value_t& origin) : ovlKeyValue(key, origin) {}
  ovlOrigin(ovlOrigin&&) = default;
  ~ovlOrigin() = default;
};

class ovlVersion final : public ovlKeyValue {
 public:
  ovlVersion(object_t::key_type const& key, value_t& version) : ovlKeyValue(key, version) {}
  ovlVersion(ovlVersion&&) = default;
  ~ovlVersion() = default;
};
class ovlConfigurableEntity final : public ovlKeyValue {
 public:
  ovlConfigurableEntity(object_t::key_type const& key, value_t& entity) : ovlKeyValue(key, entity) {}
  ovlConfigurableEntity(ovlConfigurableEntity&&) = default;
  ~ovlConfigurableEntity() = default;
};
class ovlConfiguration final : public ovlKeyValue {
 public:
  ovlConfiguration(object_t::key_type const& key, value_t& configuration) : ovlKeyValue(key, configuration) {}
  ovlConfiguration(ovlConfiguration&&) = default;
  ~ovlConfiguration() = default;
};

class ovlChangeLog final : public ovlKeyValue {
 public:
  ovlChangeLog(object_t::key_type const& key, value_t& changelog) : ovlKeyValue(key, changelog) {}

  std::string& value() { return value_as<std::string>(literal::changelog); }
  std::string& value(std::string const& changelog) {
    value() = changelog;
    return value();
  }
  std::string& append(std::string const& changelog) {
    value() += changelog;
    return value();
  }
};

class ovlConfigurations final : public ovlKeyValue {
 public:
  ovlConfigurations(object_t::key_type const& key, value_t& configurations) : ovlKeyValue(key, configurations) {}
  ovlConfigurations(ovlConfigurations&&) = default;
  ~ovlConfigurations() = default;

  void wipe() { _configurations = std::list<ovlConfiguration>{}; }
  // bool exists(std::string const& name) {}
  // void add(std::string const& name) {}
  // void remove(std::string name) {}

 private:
  std::list<ovlConfiguration> _configurations;
};

class ovlId final : public ovlKeyValue {
 public:
  ovlId(object_t::key_type const& key, value_t& oid) : ovlKeyValue(key, oid) {}
  ovlId(ovlId&&) = default;
  ~ovlId() = default;

  std::string& oid() { return value_as<std::string>(literal::oid); }
  std::string& oid(std::string const& id) {
    assert(id.empty());
    oid() = id;
    return oid();
  }
};

class ovlUpdate final : public ovlKeyValue {};

class ovlBookkeeping final : public ovlKeyValue {
 public:
  ovlBookkeeping(object_t::key_type const& key, value_t& bookkeeping)
      : ovlKeyValue(key, bookkeeping), _updates(std::move(make_updates(value(literal::updates)))) {}
  ovlBookkeeping(ovlBookkeeping&&) = default;
  ~ovlBookkeeping() = default;

  bool& isReadonly() { return value_as<bool>(literal::isreadonly); }
  bool& markReadonly(bool const& state) {
    value_as<bool>(literal::isreadonly) = state;
    return isReadonly();
  }
  bool& isDeleted() { return value_as<bool>(literal::isdeleted); }
  bool& markDeleted(bool const& state) {
    value_as<bool>(literal::isdeleted) = state;
    return isDeleted();
  }

 private:
  std::list<ovlUpdate> make_updates(value_t& updates) {
    assert(type(updates) == type_t::ARRAY);

    auto returnValue = std::list<ovlUpdate>{};
    return returnValue;
  }

 private:
  std::list<ovlUpdate> _updates;
};

class ovlDatabaseRecord final : public ovlKeyValue {
  ovlDatabaseRecord(value_t& record)
      : ovlKeyValue(literal::database_record, record),
        _document(overlay<ovlDocument>(record, literal::document_node)),
        _comments(overlay<ovlComments, array_t>(record, literal::comments_node)),
        _origin(overlay<ovlOrigin>(record, literal::origin_node)),
        _version(overlay<ovlVersion>(record, literal::version_node)),
        _configurableEntity(overlay<ovlConfigurableEntity>(record, literal::configurable_entity_node)),
        _configurations(overlay<ovlConfigurations, array_t>(record, literal::configurations_node)),
        _changelog(overlay<ovlChangeLog, std::string>(record, literal::changelog)),
        _bookkeeping(overlay<ovlBookkeeping>(record, literal::bookkeeping)),
        _id(overlay<ovlId>(record, literal::id_node)) {}

  ovlDatabaseRecord(ovlDatabaseRecord&&) = default;
  ~ovlDatabaseRecord() = default;

  ovlDocument& document() { return *_document; }
  void swap_document(std::unique_ptr<ovlDocument>& document) { std::swap(_document, document); }

  ovlComments& comments() { return *_comments; }
  void swap_comments(std::unique_ptr<ovlComments>& comments) { std::swap(_comments, comments); }

  ovlOrigin& origin() { return *_origin; }
  void swap_origin(std::unique_ptr<ovlOrigin>& origin) { std::swap(_origin, origin); }

  ovlVersion& version() { return *_version; }
  void swap_version(std::unique_ptr<ovlVersion> version) { std::swap(_version, version); }

  ovlConfigurableEntity& configurableEntity() { return *_configurableEntity; }
  void swap_configurableEntity(std::unique_ptr<ovlConfigurableEntity>& configurableEntity) {
    std::swap(_configurableEntity, configurableEntity);
  }

  ovlConfigurations& configurations() { return *_configurations; }
  void swap_configurations(std::unique_ptr<ovlConfigurations>& configurations) {
    std::swap(_configurations, configurations);
  }

  ovlBookkeeping& bookkeeping() { return *_bookkeeping; }
  void swap_bookkeeping(std::unique_ptr<ovlBookkeeping>& bookkeeping) { std::swap(_bookkeeping, bookkeeping); }

  ovlId& id() { return *_id; }
  void swap_id(std::unique_ptr<ovlId>& id) { std::swap(_id, id); }

  std::string to_string() const noexcept override {
    std::ostringstream oss;
    oss << "{";
    oss << "\"_id\":\"" << _id->to_string() << "\",\n";
    oss << "\"_version\":\"" << _version->to_string() << "\",\n";
    oss << "\"_origin\":\"" << _origin->to_string() << "\",\n";
    oss << "\"_bookkeeping\":\"" << _bookkeeping->to_string() << "\",\n";
    oss << "\"_changelog\":\"" << _changelog->to_string() << "\"";
    oss << "}";

    return oss.str();
  }

 private:
  std::unique_ptr<ovlDocument> _document;
  std::unique_ptr<ovlComments> _comments;
  std::unique_ptr<ovlOrigin> _origin;
  std::unique_ptr<ovlVersion> _version;
  std::unique_ptr<ovlConfigurableEntity> _configurableEntity;
  std::unique_ptr<ovlConfigurations> _configurations;
  std::unique_ptr<ovlChangeLog> _changelog;
  std::unique_ptr<ovlBookkeeping> _bookkeeping;
  std::unique_ptr<ovlId> _id;
};
}

}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_OVERLAY_H_ */
