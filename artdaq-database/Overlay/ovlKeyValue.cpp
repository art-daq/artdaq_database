#include "artdaq-database/Overlay/ovlKeyValue.h"

#include <utility>

using namespace artdaq::database;
using namespace artdaq::database::overlay;
using namespace artdaq::database::result;
using result_t = artdaq::database::result_t;
using artdaq::database::sharedtypes::unwrap;

std::uint32_t artdaq::database::overlay::useCompareMask(std::uint32_t compareMask) {
  static std::uint32_t _compareMask = compareMask;

  return _compareMask;
}

ovlKeyValue::ovlKeyValue(object_t::key_type key, value_t& value) : _key(std::move(key)), _value(value) {}

std::string ovlKeyValue::to_string() const {
  auto retValue = std::string{};
  auto tmpAST = object_t{};

  tmpAST[_key] = _value;

  using artdaq::database::json::JsonWriter;

  if (JsonWriter().write(tmpAST, retValue)) {
    return retValue;
  }
  { return msg_ConvertionError; }
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

result_t ovlKeyValue::operator==(ovlKeyValue const& other) const {
  if (_key != other._key) {
    return {false, "Keys are different: self,other=" + _key + "," + other._key + "."};
  }

  auto result = artdaq::database::json::operator==(_value, other._value);

  if (result.first) {
    return result;
  }

  using artdaq::database::json::print_visitor;

  std::ostringstream oss;
  oss << "\n  Values disagree.";
  oss << "\n  Key: " << quoted_(_key);
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}

result_t ovlKeyValue::swap(ovlKeyValue* other) try {
  if (other == nullptr) {
    return Failure(msg_InvalidArgument);
  }

  _key.swap(other->_key);
  _value.swap(other->_value);

  return Success();
} catch (...) {
  return Failure();
}