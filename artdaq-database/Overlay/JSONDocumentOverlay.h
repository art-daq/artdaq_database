#ifndef _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_OVERLAY_H_
#define _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_OVERLAY_H_

#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/Json/json_types_impl.h"
#include "artdaq-database/DataFormats/common/helper_functions.h"
#include "artdaq-database/JsonDocument/common.h"

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::json::array_t;
using artdaq::database::json::type_t;

namespace literal = artdaq::database::dataformats::literal;

namespace artdaq {
namespace database {
namespace overlay {

constexpr auto msg_IsReadonly = "{\"message\":\"Document is readonly\"}";
constexpr auto msg_ConvertionError = "{\"message\":\"Conversion error\"}";

constexpr auto msg_Missing = "{\"message\":\"Missing\"}";
constexpr auto msg_Ignored = "{\"message\":\"Ignored\"}";
constexpr auto msg_Updated = "{\"message\":\"Updated\"}";
constexpr auto msg_Added = "{\"message\":\"Added\"}";
constexpr auto msg_Removed = "{\"message\":\"Removed\"}";

constexpr auto msg_Success = "{\"message\":\"Success\"}";
constexpr auto msg_Failure = "{\"message\":\"Failure\"}";

using result_t = std::pair<bool, std::string>;

result_t Failure(std::ostringstream const& oss);
result_t Success(std::ostringstream const& oss);
result_t Failure(std::string const& msg = msg_Failure);
result_t Success(std::string const& msg = msg_Success);

template <typename OVL, typename T = object_t>
std::unique_ptr<OVL> overlay(value_t& /*parent*/, object_t::key_type const& /*self_key*/);

enum DOCUMENT_COMPARE_FLAGS {
  DOCUMENT_COMPARE_MUTE_TIMESTAMPS = (1 << 0),
  DOCUMENT_COMPARE_MUTE_OUIDS = (1 << 1),
  DOCUMENT_COMPARE_MUTE_COMMENTS = (1 << 2),
  DOCUMENT_COMPARE_MUTE_BOOKKEEPING = (1 << 3),
  DOCUMENT_COMPARE_MUTE_ORIGIN = (1 << 4),
  DOCUMENT_COMPARE_MUTE_VERSION = (1 << 5),
  DOCUMENT_COMPARE_MUTE_CHANGELOG = (1 << 6),
  DOCUMENT_COMPARE_MUTE_UPDATES = (1 << 7),
  DOCUMENT_COMPARE_MUTE_CONFIGENTITY = (1 << 8),
  DOCUMENT_COMPARE_MUTE_CONFIGURATIONS = (1 << 9),

  // use with caution
  DOCUMENT_COMPARE_MUTE_DATA = (1 << 29),
  DOCUMENT_COMPARE_MUTE_METADATA = (1 << 30)
};

std::uint32_t useCompareMask(std::uint32_t = 0);

class ovlKeyValue {
 public:
  ovlKeyValue(object_t::key_type const& /*key*/, value_t& /*value*/);

  // defaults
  ovlKeyValue(ovlKeyValue&&) = default;
  virtual ~ovlKeyValue() = default;

  value_t& value(object_t::key_type const& /*key*/);

  template <typename T>
  T& value_as(object_t::key_type const& /*key*/);
  template <typename T>
  T const& value_as(object_t::key_type const& /*key*/) const;

  array_t& array_value();
  object_t& object_value();
  object_t::key_type& key();
  object_t::key_type const& key() const;
  value_t& value();
  std::string& stringValue();
  std::string const& stringValue() const;

  ovlKeyValue const& self() const;
  ovlKeyValue& self();

  // virtuals
  virtual std::string to_string() const noexcept;

  // ops
  result_t operator==(ovlKeyValue const&) const;

 private:
  value_t& objectValue(object_t::key_type const& /*key*/);
  array_t& arrayValue();
  object_t& objectValue();
  std::string& objectStringValue();

  std::string const& objectStringValue() const;

  template <typename T>
  T& objectValue(object_t::key_type const& /*key*/);

  template <typename T>
  T const& objectValue(object_t::key_type const& /*key*/) const;

 private:
  object_t::key_type _key;
  value_t& _value;
};

using ovlKeyValueUPtr_t = std::unique_ptr<ovlKeyValue>;

class ovlValueTimeStamp final : public ovlKeyValue {
 public:
  ovlValueTimeStamp(object_t::key_type const& /*key*/, value_t& /*ts*/);

  // defaults
  ovlValueTimeStamp(ovlValueTimeStamp&&) = default;
  ~ovlValueTimeStamp() = default;

  // accessors
  std::string& timestamp();
  std::string const& timestamp() const;
  std::string& timestamp(std::string const& /*ts*/);

  // overridess
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlValueTimeStamp const&) const;
};
using ovlValueTimeStampUPtr_t = std::unique_ptr<ovlValueTimeStamp>;

class ovlData final : public ovlKeyValue {
 public:
  ovlData(object_t::key_type const& /*key*/, value_t& /*data*/);

  // defaults
  ovlData(ovlData&&) = default;
  ~ovlData() = default;

  // ops
  result_t operator==(ovlData const&) const;
};

using ovlDataUPtr_t = std::unique_ptr<ovlData>;

class ovlMetadata final : public ovlKeyValue {
 public:
  ovlMetadata(object_t::key_type const& /*key*/, value_t& /*metadata*/);

  // defaults
  ovlMetadata(ovlMetadata&&) = default;
  ~ovlMetadata() = default;

  // ops
  result_t operator==(ovlMetadata const&) const;
};

using ovlMetadataUPtr_t = std::unique_ptr<ovlMetadata>;

class ovlDocument final : public ovlKeyValue {
 public:
  ovlDocument(object_t::key_type const& /*key*/, value_t& /*document*/);

  // defaults
  ovlDocument(ovlDocument&&) = default;
  ~ovlDocument() = default;

  // accessors
  ovlData& data();
  ovlMetadata& metadata();

  // utils
  void swap(ovlDataUPtr_t& /*data*/);
  void swap(ovlMetadataUPtr_t& /*metadata*/);
  void make_empty();

  // overridess
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlDocument const&) const;

 private:
  ovlDataUPtr_t _data;
  ovlMetadataUPtr_t _metadata;
};

using ovlDocumentUPtr_t = std::unique_ptr<ovlDocument>;

class ovlComment final : public ovlKeyValue {
 public:
  ovlComment(object_t::key_type const& /*key*/, value_t& /*comment*/);

  // defaults
  ovlComment(ovlComment&&) = default;
  ~ovlComment() = default;

  // accessors
  int& linenum();
  std::string& text();
  int const& linenum() const;
  std::string const& text() const;

  // overridess
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlComment const&) const;
};

using ovlCommentUPtr_t = std::unique_ptr<ovlComment>;

class ovlComments final : public ovlKeyValue {
  using arrayComments_t = array_t::container_type<ovlComment>;

 public:
  ovlComments(object_t::key_type const& /*key*/, value_t& /*comments*/);

  // defaults
  ovlComments(ovlComments&&) = default;
  ~ovlComments() = default;

  // accessors

  // utils
  void wipe();

  // overridess
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlComments const&) const;

 private:
  arrayComments_t make_comments(array_t& /*comments*/);

 private:
  arrayComments_t _comments;
};

using ovlCommentsUPtr_t = std::unique_ptr<ovlComments>;

class ovlOrigin final : public ovlKeyValue {
 public:
  ovlOrigin(object_t::key_type const& /*key*/, value_t& /*origin*/);

  // defaults
  ovlOrigin(ovlOrigin&&) = default;
  ~ovlOrigin() = default;

  // accessors
  std::string& format();
  std::string const& format() const;
  std::string& source();
  std::string const& source() const;

  // overridess
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlOrigin const&) const;

 private:
  ovlValueTimeStamp map_timestamp(value_t& /*value*/);

 private:
  ovlValueTimeStamp _timestamp;
};

using ovlOriginUPtr_t = std::unique_ptr<ovlOrigin>;

class ovlVersion final : public ovlKeyValue {
 public:
  ovlVersion(object_t::key_type const& /*key*/, value_t& /*version*/);

  // defaults
  ovlVersion(ovlVersion&&) = default;
  ~ovlVersion() = default;

  // ops
  result_t operator==(ovlVersion const&) const;
};

using ovlVersionUPtr_t = std::unique_ptr<ovlVersion>;

class ovlAlias final : public ovlKeyValue {
 public:
  ovlAlias(object_t::key_type const& /*key*/, value_t& /*alias*/);

  // defaults
  ovlAlias(ovlAlias&&) = default;
  ~ovlAlias() = default;

  // accessors
  std::string& name();
  std::string const& name() const;
  std::string& name(std::string const& /*name*/);
  std::string& timestamp();

  // overrides
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlAlias const&) const;

 private:
  ovlValueTimeStamp map_timestamp(value_t& /*value*/);

 private:
  ovlValueTimeStamp _timestamp;
};

using ovlAliasUPtr_t = std::unique_ptr<ovlAlias>;

class ovlConfigurableEntity final : public ovlKeyValue {
 public:
  ovlConfigurableEntity(object_t::key_type const& key, value_t& /*entity*/);

  // defaults
  ovlConfigurableEntity(ovlConfigurableEntity&&) = default;
  ~ovlConfigurableEntity() = default;

  // ops
  result_t operator==(ovlConfigurableEntity const&) const;
};

using ovlConfigurableEntityUPtr_t = std::unique_ptr<ovlConfigurableEntity>;

class ovlConfiguration final : public ovlKeyValue {
 public:
  ovlConfiguration(object_t::key_type const& /*key*/, value_t& /*configuration*/);

  // defaults
  ovlConfiguration(ovlConfiguration&&) = default;
  ~ovlConfiguration() = default;

  // accessors
  std::string& name();
  std::string const& name() const;
  std::string& name(std::string const& /*name*/);
  std::string& timestamp();

  // overrides
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlConfiguration const&) const;

 private:
  ovlValueTimeStamp map_timestamp(value_t& /*value*/);

 private:
  ovlValueTimeStamp _timestamp;
};

using ovlConfigurationUPtr_t = std::unique_ptr<ovlConfiguration>;

class ovlChangeLog final : public ovlKeyValue {
 public:
  ovlChangeLog(object_t::key_type const& /*key*/, value_t& /*changelog*/);

  // defaults
  ovlChangeLog(ovlChangeLog&&) = default;
  ~ovlChangeLog() = default;

  // accessors
  std::string& value();
  std::string& value(std::string const& /*changelog*/);

  // utils
  std::string& append(std::string const& /*changelog*/);

  // ops
  result_t operator==(ovlChangeLog const&) const;
};

using ovlChangeLogUPtr_t = std::unique_ptr<ovlChangeLog>;

class ovlConfigurations final : public ovlKeyValue {
  using arrayConfigurations_t = array_t::container_type<ovlConfiguration>;

 public:
  ovlConfigurations(object_t::key_type const& /*key*/, value_t& /*configurations*/);

  // defaults
  ovlConfigurations(ovlConfigurations&&) = default;
  ~ovlConfigurations() = default;

  // utils
  void wipe();
  // bool exists(std::string const& name) {}
  result_t add(ovlConfigurationUPtr_t&);
  result_t remove(ovlConfigurationUPtr_t&);

  // overrides
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlConfigurations const&) const;

 private:
  arrayConfigurations_t make_configurations(array_t& /*configurations*/);

 private:
  arrayConfigurations_t _configurations;
};
using ovlConfigurationsUPtr_t = std::unique_ptr<ovlConfigurations>;

class ovlId final : public ovlKeyValue {
 public:
  ovlId(object_t::key_type const& /*key*/, value_t& /*oid*/);

  // defaults
  ovlId(ovlId&&) = default;
  ~ovlId() = default;

  // accessors
  std::string& oid();
  std::string& oid(std::string const& /*id*/);

  // ops
  result_t operator==(ovlId const&) const;
};

using ovlIdUPtr_t = std::unique_ptr<ovlId>;

class ovlUpdate final : public ovlKeyValue {
 public:
  ovlUpdate(object_t::key_type const& /*key*/, value_t& /*update*/);

  // defaults
  ovlUpdate(ovlUpdate&&) = default;
  ~ovlUpdate() = default;

  // accessors
  std::string& opration();
  std::string& timestamp();

  // overrides
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlUpdate const&) const;

 private:
  ovlValueTimeStamp map_operation(value_t& /*value*/);

 private:
  ovlValueTimeStamp _operation;
};

using ovlUpdateUPtr_t = std::unique_ptr<ovlUpdate>;

class ovlBookkeeping final : public ovlKeyValue {
  using arrayBookkeeping_t = array_t::container_type<ovlUpdate>;

 public:
  ovlBookkeeping(object_t::key_type const& /*key*/, value_t& /*bookkeeping*/);

  // defaults
  ovlBookkeeping(ovlBookkeeping&&) = default;
  ~ovlBookkeeping() = default;

  // accessors
  bool& isReadonly();
  bool const& isReadonly() const;
  bool& isDeleted();
  bool const& isDeleted() const;

  // utils
  bool& markReadonly(bool const& /*state*/);
  bool& markDeleted(bool const& /*state*/);

  result_t postUpdate(std::string const& /*update*/);

  // overrides
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlBookkeeping const&) const;

 private:
  arrayBookkeeping_t make_updates(array_t& /*updates*/);
  ovlValueTimeStamp map_created(value_t& /*value*/);

 private:
  arrayBookkeeping_t _updates;
  ovlValueTimeStamp _created;
};

using ovlBookkeepingUPtr_t = std::unique_ptr<ovlBookkeeping>;

class ovlDatabaseRecord final : public ovlKeyValue {
 public:
  ovlDatabaseRecord(value_t& record);

  // defaults
  ovlDatabaseRecord(ovlDatabaseRecord&&) = default;
  ~ovlDatabaseRecord() = default;

  // accessors
  ovlDocument& document();
  ovlComments& comments();
  ovlOrigin& origin();
  ovlVersion& version();
  ovlConfigurableEntity& configurableEntity();
  ovlConfigurations& configurations();
  ovlBookkeeping& bookkeeping();
  ovlId& id();

  // utils
  result_t swap(ovlDocumentUPtr_t& /*document*/);
  result_t swap(ovlCommentsUPtr_t& /*comments*/);
  result_t swap(ovlOriginUPtr_t& /*origin*/);
  result_t swap(ovlVersionUPtr_t& /*version*/);
  result_t swap(ovlConfigurationsUPtr_t& /*configurations*/);
  result_t swap(ovlBookkeepingUPtr_t& /*bookkeeping*/);
  result_t swap(ovlIdUPtr_t& /*id*/);
  result_t swap(ovlConfigurableEntityUPtr_t& /*entity*/);

  // overrides
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlDatabaseRecord const&) const;

  // delegates
  bool& isReadonly();
  bool const& isReadonly() const;
  result_t markReadonly();
  result_t markDeleted();

  result_t addConfiguration(ovlConfigurationUPtr_t& /*configuration*/);
  result_t removeConfiguration(ovlConfigurationUPtr_t& /*configuration*/);

  //  result_t addAlias(ovlAliasUPtr_t& /*alias*/);
  //  result_t removeAlias(ovlAliasUPtr_t& /*alias*/);

  result_t addConfigurableEntity(ovlConfigurableEntityUPtr_t& /*entity*/);
  result_t removeConfigurableEntity(ovlConfigurableEntityUPtr_t& /*entity*/);

  result_t setVersion(ovlVersionUPtr_t& /*version*/);

 private:
  std::string make_update(std::string const& update) const { return update; }

 private:
  ovlDocumentUPtr_t _document;
  ovlCommentsUPtr_t _comments;
  ovlOriginUPtr_t _origin;
  ovlVersionUPtr_t _version;
  ovlConfigurableEntityUPtr_t _configurableEntity;
  ovlConfigurationsUPtr_t _configurations;
  ovlChangeLogUPtr_t _changelog;
  ovlBookkeepingUPtr_t _bookkeeping;
  ovlIdUPtr_t _id;
};
}

std::string quoted_(std::string const& /*text*/);
std::string quoted_(bool const& /*bool*/);

std::string operator"" _quoted(const char* /*text*/, std::size_t);
std::string debrace(std::string /*s*/);

}  // namespace database
}  // namespace artdaq

namespace ovl = artdaq::database::overlay;

template <typename OVL, typename T = object_t>
std::unique_ptr<OVL> ovl::overlay(value_t& parent, object_t::key_type const& self_key) {
  assert(!self_key.empty());
  assert(type(parent) == type_t::OBJECT);

  using artdaq::database::sharedtypes::unwrap;

  if (self_key.empty()) throw std::runtime_error("Errror: self_key is empty");

  if (type(parent) != type_t::OBJECT) throw std::runtime_error("Errror: parent is not a type_t::OBJECT type");

  return std::make_unique<OVL>(self_key, unwrap(parent).value<object_t, T>(self_key));
}

template <typename T>
T& ovl::ovlKeyValue::value_as(object_t::key_type const& key) {
  return objectValue<T>(key);
}

template <typename T>
T const& ovl::ovlKeyValue::value_as(object_t::key_type const& key) const {
  return objectValue<T>(key);
}

template <typename T>
T& ovl::ovlKeyValue::objectValue(object_t::key_type const& key) {
  using artdaq::database::sharedtypes::unwrap;

  return unwrap(_value).value_as<T>(key);
}

template <typename T>
T const& ovl::ovlKeyValue::objectValue(object_t::key_type const& key) const {
  using artdaq::database::sharedtypes::unwrap;

  return unwrap(_value).value_as<T>(key);
}

#endif /* _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_OVERLAY_H_ */
