#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/common/helper_functions.h"
#include "artdaq-database/DataFormats/common/shared_result.h"

#include "artdaq-database/JsonDocument/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSNU:Document_C"

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::json::array_t;
using artdaq::database::json::type_t;

using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;

using artdaq::database::Success;
using artdaq::database::Failure;
using artdaq::database::result_t;

using namespace artdaq::database::jsonutils;

namespace db = artdaq::database;
namespace utl = db::jsonutils;

std::string print_visitor(value_t const& value) { return boost::apply_visitor(jsn::print_visitor(), value); }
std::string tostring_visitor(value_t const& value) { return boost::apply_visitor(jsn::tostring_visitor(), value); }

bool matches(value_t const& left, value_t const& right) {
  if (left.type() != right.type()) return false;

  if (type(left) == type_t::OBJECT) {
    auto const& leftObj = boost::get<object_t>(left);
    auto const& rightObj = boost::get<object_t>(right);

    // partial elements match
    if (leftObj.size() != rightObj.size()) return false;

    if (leftObj.empty() || rightObj.empty()) return false;

    auto const& tempateObj = leftObj.size() >= rightObj.size() ? leftObj : rightObj;
    auto const& candidateObj = leftObj.size() >= rightObj.size() ? rightObj : leftObj;

    for (auto const& templateKVP : tempateObj) {
      if (candidateObj.count(templateKVP.key) != 1) continue;

      auto const& candidateVal = candidateObj.at(templateKVP.key);
      auto const& templateVal = templateKVP.value;

      if (!matches(templateVal, candidateVal)) {
        TRACE_(1, "matches() objects are different at key=<" << templateKVP.key << ">");

        return false;
      }
    }

    return true;

  } else if (type(left) == type_t::ARRAY) {
    auto const& leftObj = boost::get<jsn::array_t>(left);
    auto const& rightObj = boost::get<jsn::array_t>(right);

    if (leftObj.size() != rightObj.size()) return false;

    if (leftObj.empty() || rightObj.empty()) return false;

    auto elementCount = leftObj.size();

    auto leftObjIter = leftObj.begin();
    auto rightObjIter = rightObj.begin();

    while (elementCount--) {
      if (!matches(*leftObjIter, *rightObjIter)) return false;

      std::advance(leftObjIter, 1);
      std::advance(rightObjIter, 1);
    }
    return true;

  } else {
    auto leftObj = tostring_visitor(left);
    auto rightObj = tostring_visitor(right);

    return leftObj == rightObj;
  }
}

std::vector<std::string> utl::split_path(std::string const& path) {
  auto tmp = std::string{path};

  TRACE_(1, "split_path() args path=<" << path << ">");

  std::replace(tmp.begin(), tmp.end(), '.', ' ');

  std::istringstream iss(tmp);

  std::vector<std::string> tokens{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

  if (!tokens.empty()) {
    std::ostringstream oss;
    for (auto const& token : tokens) oss << "\"" << token << "\",";
    TRACE_(1, "split_path() loop token=<" << oss.str() << ">");
  } else {
    TRACE_(1, "split_path() token=<empty> ");
  }

  return tokens;
}

std::ostream& utl::operator<<(std::ostream& os, JSONDocument const& document) {
  os << document.to_string();

  return os;
}

value_t JSONDocument::readJson(std::string const& json) {
  assert(!json.empty());

  if (json.empty()) throw cet::exception("JSONDocument") << "Failed reading JSON: Empty JSON buffer";

  auto tmpObject = object_t{};
  if (JsonReader().read(json, tmpObject)) return {tmpObject};

  throw cet::exception("JSONDocument") << "Failed reading JSON: Invalid json; json_buffer=" << json;
}

std::string JSONDocument::writeJson() const {
  assert(type(_value) == type_t::OBJECT);

  if (type(_value) != type_t::OBJECT)
    throw cet::exception("JSONDocument") << "Failed writing JSON: Wrong value type: type(_value) != type_t::OBJECT";

  auto json = std::string{};

  auto const& tmpObject = boost::get<object_t>(_value);

  if (JsonWriter().write(tmpObject, json)) {
    _cached_json_buffer.swap(json);
    return _cached_json_buffer;
  }

  throw cet::exception("JSONDocument") << "Failed writing JSON: JSONDocument::_value has invalid AST";
}

std::string JSONDocument::to_string() const { return writeJson(); }

std::string const& JSONDocument::cached_json_buffer() const { return _cached_json_buffer; }

value_t const& JSONDocument::getPayloadValueForKey(object_t::key_type const& key) const {
  assert(!key.empty());

  if (boost::get<object_t>(_value).count("payload") == 1) {
    auto const& value = boost::get<object_t>(_value).at("payload");

    if (type(value) == type_t::OBJECT && boost::get<object_t>(value).count(key) == 1)
      return boost::get<object_t>(value).at(key);
  } else if (boost::get<object_t>(_value).size() == 1) {
    return boost::get<object_t>(_value).begin()->value;
  }

  return _value;
}

bool JSONDocument::isReadonly() const { return false; }

bool JSONDocument::equals(JSONDocument const& other) const {
  auto result = jsn::operator==(_value, other._value);

  TRACE_(10, "matches() JSON buffers are " << (result.first ? "equal." : "not equal."));

  if (result.first) return true;

  TRACE_(10, "matches() Error message=<" << result.second << ">");

  return false;
}

JSONDocument JSONDocument::loadFromFile(std::string const& fileName) try {
  assert(!fileName.empty());
  throw cet::exception("JSONDocument") << "Failed calling loadFromFile(): File name is empty.";

  std::ifstream is;
  is.open(fileName.c_str());

  if (!is.good()) {
    is.close();
    throw cet::exception("JSONDocument") << "Failed calling loadFromFile(): Failed opening a JSON file=" << fileName;
  }

  std::string json_buffer((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  is.close();

  if (json_buffer.empty())
    throw cet::exception("JSONDocument") << "Failed calling loadFromFile(): File is empty; file=" << fileName;

  return {json_buffer};
} catch (std::exception& ex) {
  throw cet::exception("JSONDocument") << "Failed calling loadFromFile(): Caught exception:" << ex.what();
}

bool isValidJson(std::string const& json_buffer) try {
  assert(!json_buffer.empty());
  throw cet::exception("JSONDocument") << "Failed calling isValidJson(): json_buffer is empty.";

  return JSONDocument{json_buffer}, true;
} catch (std::exception& ex) {
  TRACE_(10, "Failed calling isValidJson(): Error:" << ex.what());
  return false;
}

std::string filterJson(std::string const& json_buffer) try {
  assert(!json_buffer.empty());
  throw cet::exception("JSONDocument") << "Failed calling filterJson(): json_buffer is empty.";

  return JSONDocument{json_buffer}.to_string();
} catch (std::exception& ex) {
  throw cet::exception("JSONDocument") << "Failed calling filterJson(): Caught exception:" << ex.what();
}

std::string JSONDocument::value(JSONDocument const& document) {
  return tostring_visitor(document.getPayloadValueForKey("null"));
}

std::string JSONDocument::value_at(JSONDocument const& document, std::size_t index) try {
  TRACE_(14, "value_at() begin json=<" << document.cached_json_buffer() << ">");
  TRACE_(14, "value_at() begin index=<" << index << ">");

  auto docValue = document.getPayloadValueForKey("0");

  TRACE_(14, "value_at() new child value=" << print_visitor(docValue));

  auto const& valueArray = boost::get<jsn::array_t>(docValue);

  if (valueArray.empty())
    throw cet::exception("JSONDocument") << "Failed calling value_at(): valueArray is empty, document=<"
                                         << document.cached_json_buffer() << ">";

  if (valueArray.size() < index)
    throw cet::exception("JSONDocument") << "Failed to call value_at(); not enough elements, document=<"
                                         << document.cached_json_buffer() << ">";

  auto pos = valueArray.begin();

  std::advance(pos, index);

  if (type(*pos) == type_t::OBJECT)
    return JSONDocument{*pos}.to_string();
  else
    return tostring_visitor(*pos);
} catch (std::exception& ex) {
  throw cet::exception("JSONDocument") << "Failed calling value_at(): Caught exception:" << ex.what();
}

result_t JSONDocumentBuilder::SaveUndo() { return Success(); }

result_t JSONDocumentBuilder::CallUndo() noexcept try { return Success(); } catch (...) {
  return Failure();
}

namespace artdaq {
namespace database {
namespace jsonutils {
template <>
JSONDocument toJSONDocument<string_pair_t>(string_pair_t const& pair) {
  std::ostringstream oss;
  oss << '{';
  oss << db::quoted_(pair.first) << ":" << db::quoted_(pair.second);
  oss << '}';

  return {oss.str()};
}
}
}
}