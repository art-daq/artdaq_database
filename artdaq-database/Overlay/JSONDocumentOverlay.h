#ifndef _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_OVERLAY_H_
#define _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_OVERLAY_H_

#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/Json/json_types_impl.h"
#include "artdaq-database/JsonDocument/common.h"

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::json::array_t;
using artdaq::database::json::type_t;

namespace literal = artdaq::database::dataformats::literal;

namespace artdaq {
namespace database {
namespace internals {

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

  //use with caution
  DOCUMENT_COMPARE_MUTE_DATA = (1 << 29),
  DOCUMENT_COMPARE_MUTE_METADATA = (1 << 30)
};

std::uint32_t useCompareMask(std::uint32_t =0);

class ovlKeyValue {
 public:
  ovlKeyValue(object_t::key_type const& /*key*/, value_t& /*value*/);
  
  // defaults
  ovlKeyValue(ovlKeyValue&&) = default;
  virtual ~ovlKeyValue()=default;

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

  ovlKeyValue const& self()const;
  ovlKeyValue & self();
  
  // virtuals
  virtual std::string to_string() const noexcept;
  
  //ops
  std::pair<bool, std::string> operator==(ovlKeyValue const&) const;

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

  //ops
  std::pair<bool, std::string> operator==(ovlValueTimeStamp const&) const;
};

class ovlData final : public ovlKeyValue {
 public:
  ovlData(object_t::key_type const& /*key*/, value_t& /*data*/);

  // defaults
  ovlData(ovlData&&) = default;
  ~ovlData() = default;

  //ops
  std::pair<bool, std::string> operator==(ovlData const&) const;    
};

class ovlMetadata final : public ovlKeyValue {
 public:
  ovlMetadata(object_t::key_type const& /*key*/, value_t& /*metadata*/);

  // defaults
  ovlMetadata(ovlMetadata&&) = default;
  ~ovlMetadata() = default;

  //ops
  std::pair<bool, std::string> operator==(ovlMetadata const&) const;      
};
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
  void swap_data(std::unique_ptr<ovlData>& /*data*/);
  void swap_metadata(std::unique_ptr<ovlMetadata>& /*metadata*/);
  void make_empty();

  // overridess
  std::string to_string() const noexcept override;

  //ops
  std::pair<bool, std::string> operator==(ovlDocument const&) const;      

 private:
  std::unique_ptr<ovlData> _data;
  std::unique_ptr<ovlMetadata> _metadata;
};

class ovlComment final : public ovlKeyValue {
 public:
  ovlComment(object_t::key_type const& /*key*/, value_t& /*comment*/, array_t& /*parent*/);

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

  //ops
  std::pair<bool, std::string> operator==(ovlComment const&) const;      
  
 private:
  array_t& _parent;
};

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

  //ops
  std::pair<bool, std::string> operator==(ovlComments const&) const;      
  
 private:
  arrayComments_t make_comments(array_t& /*comments*/);

 private:
  arrayComments_t _comments;
};

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

  //ops
  std::pair<bool, std::string> operator==(ovlOrigin const&) const;      
  
 private:
  ovlValueTimeStamp map_timestamp(value_t& /*value*/);

 private:
  ovlValueTimeStamp _timestamp;
};

class ovlVersion final : public ovlKeyValue {
 public:
  ovlVersion(object_t::key_type const& /*key*/, value_t& /*version*/);

  // defaults
  ovlVersion(ovlVersion&&) = default;
  ~ovlVersion() = default;
  
  //ops
  std::pair<bool, std::string> operator==(ovlVersion const&) const;        
};

class ovlConfigurableEntity final : public ovlKeyValue {
 public:
  ovlConfigurableEntity(object_t::key_type const& key, value_t& /*entity*/);

  // defaults
  ovlConfigurableEntity(ovlConfigurableEntity&&) = default;
  ~ovlConfigurableEntity() = default;
  
  //ops
  std::pair<bool, std::string> operator==(ovlConfigurableEntity const&) const;        
};

class ovlConfiguration final : public ovlKeyValue {
 public:
  ovlConfiguration(object_t::key_type const& /*key*/, value_t& /*configuration*/, array_t& /*configurations*/);

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

  //ops
  std::pair<bool, std::string> operator==(ovlConfiguration const&) const;        
  
 private:
  ovlValueTimeStamp map_timestamp(value_t& /*value*/);

 private:
  array_t& _parent;
  ovlValueTimeStamp _timestamp;
};

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

  //ops
  std::pair<bool, std::string> operator==(ovlChangeLog const&) const;        
  
};

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
  // void add(std::string const& name) {}
  // void remove(std::string name) {}
  
  // overrides
  std::string to_string() const noexcept override;

  //ops
  std::pair<bool, std::string> operator==(ovlConfigurations const&) const;        
  
 private:
  arrayConfigurations_t make_configurations(array_t& /*configurations*/);

 private:
  arrayConfigurations_t _configurations;
};

class ovlId final : public ovlKeyValue {
 public:
  ovlId(object_t::key_type const& /*key*/, value_t& /*oid*/);

  // defaults
  ovlId(ovlId&&) = default;
  ~ovlId() = default;

  // accessors
  std::string& oid();
  std::string& oid(std::string const& /*id*/);
  
  //ops
  std::pair<bool, std::string> operator==(ovlId const&) const;        
};

class ovlUpdate final : public ovlKeyValue {
 public:
  ovlUpdate(object_t::key_type const& /*key*/, value_t& /*update*/, array_t& /*parent*/);

  // defaults
  ovlUpdate(ovlUpdate&&) = default;
  ~ovlUpdate() = default;

  // accessors
  std::string& opration();
  std::string& timestamp();

  // overrides
  std::string to_string() const noexcept override;

  //ops
  std::pair<bool, std::string> operator==(ovlUpdate const&) const;        
  
 private:
  ovlValueTimeStamp map_operation(value_t& /*value*/);

 private:
  array_t& _parent;
  ovlValueTimeStamp _operation;
};

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

  // overrides
  std::string to_string() const noexcept override;

  //ops
  std::pair<bool, std::string> operator==(ovlBookkeeping const&) const;        
  
 private:
  arrayBookkeeping_t make_updates(array_t& /*updates*/);
  ovlValueTimeStamp map_created(value_t& /*value*/);

 private:
  arrayBookkeeping_t _updates;
  ovlValueTimeStamp _created;
};

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
  void swap_document(std::unique_ptr<ovlDocument>& /*document*/);
  void swap_comments(std::unique_ptr<ovlComments>& /*comments*/);
  void swap_origin(std::unique_ptr<ovlOrigin>& /*origin*/);
  void swap_version(std::unique_ptr<ovlVersion>& /*version*/);
  void swap_configurations(std::unique_ptr<ovlConfigurations>& /*configurations*/);
  void swap_bookkeeping(std::unique_ptr<ovlBookkeeping>& /*bookkeeping*/);
  void swap_id(std::unique_ptr<ovlId>& /*id*/);
  void swap_configurableEntity(std::unique_ptr<ovlConfigurableEntity>& /*configurableEntity*/);

  // overrides
  std::string to_string() const noexcept override;

  //ops
  std::pair<bool, std::string> operator==(ovlDatabaseRecord const&) const;        
  
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

std::string quoted_(std::string const& /*text*/);
std::string operator"" _quoted(const char* /*text*/, std::size_t);
std::string debrace(std::string /*s*/);

}  // namespace database
}  // namespace artdaq

namespace ovl = artdaq::database::internals;

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
