#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUE_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUE_H_

#include "artdaq-database/Overlay/common.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

class ovlKeyValue {
 public:
  ovlKeyValue(object_t::key_type /*key*/, value_t& /*value*/);

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
  value_t const& value() const;
  std::string& string_value();
  std::string const& string_value() const;

  ovlKeyValue const& self() const;
  ovlKeyValue& self();

  // virtuals
  virtual std::string to_string() const;

  // ops
  result_t operator==(ovlKeyValue const&) const;
  result_t swap(ovlKeyValue*);

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

template <typename T>
T& ovlKeyValue::value_as(object_t::key_type const& key) {
  return objectValue<T>(key);
}

template <typename T>
T const& ovlKeyValue::value_as(object_t::key_type const& key) const {
  return objectValue<T>(key);
}

template <typename T>
T& ovlKeyValue::objectValue(object_t::key_type const& key) {
  using artdaq::database::sharedtypes::unwrap;

  return unwrap(_value).template value_as<T>(key);
}

template <typename T>
T const& ovlKeyValue::objectValue(object_t::key_type const& key) const {
  using artdaq::database::sharedtypes::unwrap;

  return unwrap(_value).template value_as<T>(key);
}

template <typename OVL, typename T = object_t>
std::unique_ptr<OVL> overlay(value_t& parent, object_t::key_type const& self_key) {
  confirm(!self_key.empty());
  confirm(type(parent) == type_t::OBJECT);

  using artdaq::database::sharedtypes::unwrap;

  if (self_key.empty()) throw std::runtime_error("Errror: self_key is empty");

  if (type(parent) != type_t::OBJECT) throw std::runtime_error("Errror: parent is not a type_t::OBJECT type");

  return std::make_unique<OVL>(self_key, unwrap(parent).template value<object_t, T>(self_key));
}

using ovlKeyValueUPtr_t = std::unique_ptr<ovlKeyValue>;
}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUE_H_ */
