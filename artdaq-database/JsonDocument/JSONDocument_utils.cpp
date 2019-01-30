#include "artdaq-database/BasicTypes/data_json.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/common.h"

#include <boost/filesystem.hpp>
#include <utility>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSONDocument_utils.cpp"

using artdaq::database::json::array_t;
using artdaq::database::json::object_t;
using artdaq::database::json::type_t;
using artdaq::database::json::value_t;

using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;

using artdaq::database::Failure;
using artdaq::database::result_t;
using artdaq::database::Success;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::sharedtypes::unwrap;

namespace db = artdaq::database;
namespace utl = db::docrecord;

namespace dbdr = artdaq::database::docrecord;

namespace jsonliteral = artdaq::database::dataformats::literal;

std::string print_visitor(value_t const& value) { return boost::apply_visitor(jsn::print_visitor(), value); }
std::string tostring_visitor(value_t const& value) { return boost::apply_visitor(jsn::tostring_visitor(), value); }

bool matches(value_t const& left, value_t const& right) {
  if (left.type() != right.type()) {
    return false;
  }

  if (type(left) == type_t::OBJECT) {
    auto const& leftObj = unwrap(left).value_as<const object_t>();
    auto const& rightObj = unwrap(right).value_as<const object_t>();

    // FIXME:GAL partial elements match
    // if (leftObj.size() != rightObj.size()) return false;

    if (leftObj.empty() || rightObj.empty()) {
      return false;
    }

    auto const& tempateObj = leftObj.size() >= rightObj.size() ? leftObj : rightObj;
    auto const& candidateObj = leftObj.size() >= rightObj.size() ? rightObj : leftObj;

    for (auto const& templateKVP : tempateObj) {
      if (candidateObj.count(templateKVP.key) != 1) {
        continue;
      }

      auto const& candidateVal = candidateObj.at(templateKVP.key);
      auto const& templateVal = templateKVP.value;

      if (!matches(templateVal, candidateVal)) {
        TLOG(20) << "matches() objects are different at key=<" << templateKVP.key << ">";

        return false;
      }
    }

    return true;
  }
  if (type(left) == type_t::ARRAY) {
    auto const& leftObj = unwrap(left).value_as<const array_t>();
    auto const& rightObj = unwrap(left).value_as<const array_t>();

    if (leftObj.size() != rightObj.size()) {
      return false;
    }

    if (leftObj.empty() || rightObj.empty()) {
      return false;
    }

    auto elementCount = leftObj.size();

    auto leftObjIter = leftObj.begin();
    auto rightObjIter = rightObj.begin();

    while ((elementCount--) != 0u) {
      if (!matches(*leftObjIter, *rightObjIter)) {
        return false;
      }

      std::advance(leftObjIter, 1);
      std::advance(rightObjIter, 1);
    }
    return true;
  }
  auto leftObj = tostring_visitor(left);
  auto rightObj = tostring_visitor(right);

  return leftObj == rightObj;
}

std::vector<std::string> utl::split_path(std::string const& path) {
  auto tmp = std::string{path};

  std::replace(tmp.begin(), tmp.end(), '.', ' ');

  std::istringstream iss(tmp);

  std::vector<std::string> tokens{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

  if (!tokens.empty()) {
    std::ostringstream oss;
    for (auto const& token : tokens) {
      oss << "\"" << token << "\",";
    }
  }

  return tokens;
}

std::ostream& utl::operator<<(std::ostream& os, JSONDocument const& document) {
  os << document.to_string();

  return os;
}

value_t JSONDocument::readJson(std::string const& json) {
  if (json.empty()) {
    throw invalid_argument("JSONDocument") << "Failed reading JSON: Empty JSON buffer";
  }

  auto tmpObject = object_t{};

  if (JsonReader().read(json, tmpObject)) {
    return {tmpObject};
  }

  throw invalid_argument("JSONDocument") << "Failed reading JSON: Invalid json; json_buffer=" << json;
}

std::string JSONDocument::writeJson() const {
  if (type(_value) != type_t::OBJECT) {
    throw invalid_argument("JSONDocument") << "Failed writing JSON: Wrong value type: type(_value) != type_t::OBJECT";
  }

  auto const& tmpObject = boost::get<object_t>(_value);

  if (tmpObject.empty()) {
    return jsonliteral::empty_json;
  }

  auto json = std::string{};

  if (!JsonWriter().write(tmpObject, json)) {
    throw invalid_argument("JSONDocument") << "Failed writing JSON: JSONDocument::_value has invalid AST";
  }

  return json;
}

std::string JSONDocument::to_string() const {
  if (_isDirty) {
    _cached_json_buffer = writeJson();
    _isDirty = false;
  }
  return _cached_json_buffer;
}

JSONDocument::operator std::string() const {
  if (_isDirty) {
    _cached_json_buffer = writeJson();
    _isDirty = false;
  }
  return _cached_json_buffer;
}

bool JSONDocument::empty() const { return _value.empty(); }

JSONDocument::JSONDocument(JsonData const& data) : _value{readJson(data)}, _cached_json_buffer(jsonliteral::empty_json), _isDirty(true) {}

JSONDocument::JSONDocument(std::string const& json) : _value{readJson(json)}, _cached_json_buffer(jsonliteral::empty_json), _isDirty(true) {}

JSONDocument::JSONDocument(value_t value) : _value{std::move(value)}, _cached_json_buffer(jsonliteral::empty_json), _isDirty(true) {}

JSONDocument::JSONDocument() : _value{object_t{}}, _cached_json_buffer(jsonliteral::empty_json), _isDirty(true) {}

std::string const& JSONDocument::cached_json_buffer() const { return _cached_json_buffer; }

value_t const& JSONDocument::getPayloadValueForKey(object_t::key_type const& key) const {
  confirm(!key.empty());

  TLOG(21) << "getPayloadValueForKey() document=<" << cached_json_buffer() << ">";

  if (unwrap(_value).value_as<const object_t>().count("payload") == 1) {
    auto const& value = unwrap(_value).value<const object_t>("payload");

    if (type(value) == type_t::OBJECT && unwrap(value).value_as<const object_t>().count(key) == 1) {
      return unwrap(value).value<const object_t>(key);
    }
    return value;
  } else if (unwrap(_value).value_as<const object_t>().size() == 1) {
    return unwrap(_value).value_as<const object_t>().begin()->value;
  }

  return _value;
}

bool JSONDocument::equals(JSONDocument const& other) const {
  auto result = jsn::operator==(_value, other._value);

  TLOG(22) << "matches() JSON buffers are " << (result.first ? "equal." : "not equal.");

  if (result.first) {
    return true;
  }

  TLOG(23) << "matches() Error message=<" << result.second << ">";

  return false;
}

JSONDocument JSONDocument::loadFromFile(std::string const& fileName) try {
  auto json_buffer = std::string{};

  if (!db::read_buffer_from_file(json_buffer, fileName)) {
    throw invalid_argument("JSONDocument") << "Failed calling loadFromFile(): Failed opening a JSON file=" << fileName;
  }

  return {json_buffer};
} catch (std::exception& ex) {
  throw runtime_error("JSONDocument") << "Failed calling loadFromFile(): Caught exception:" << ex.what();
}

bool JSONDocument::saveToFile(std::string const& fileName) try {
  if (fileName.empty()) {
    throw invalid_argument("JSONDocument") << "Failed calling saveToFile(): File name is empty.";
  }

  if (boost::filesystem::exists(fileName)) {
    boost::filesystem::copy_file(fileName, std::string{fileName} + ".bak", boost::filesystem::copy_option::overwrite_if_exists);
  }

  auto buffer = to_string();

  return db::write_buffer_to_file(buffer, fileName);
} catch (std::exception& ex) {
  throw runtime_error("JSONDocument") << "Failed calling saveToFile(): Caught exception:" << ex.what();
}

JSONDocumentBuilder& JSONDocumentBuilder::createFromData(JSONDocument doc) {
  _overlay.reset(nullptr);

  auto const document = std::move(doc);

  TLOG(24) << "createFrom() begin args";

#ifdef EXTRA_TRACES
  TLOG(24) << "createFrom() document=<" << document << ">";
#endif

  _createFromTemplate({std::string{template__empty_document}});

  {  // create a new document template using overlay classes
    auto ovl = std::make_unique<ovlDatabaseRecord>(_document._value);
    std::swap(_overlay, ovl);
  }

  _importUserData(document);

  {  // refresh overlays
    auto ovl = std::make_unique<ovlDatabaseRecord>(_document._value);
    std::swap(_overlay, ovl);
  }

  TLOG(24) << "createFrom() end";

  return self();
}

void JSONDocumentBuilder::_importUserData(JSONDocument const& document) {
  TLOG(25) << "_importUserData() begin";

#ifdef EXTRA_TRACES
  TLOG(25) << "_importUserData() document=<" << document << ">";
#endif

  // replace metadata if any
  try {
    auto path = ""s + jsonliteral::document + jsonliteral::dot + jsonliteral::metadata;
    auto metadata = document.findChild(path);

    TLOG(26) << "_importUserData() Found document.metadata=<" << metadata << ">";

    _document.replaceChild(metadata, path);

  } catch (notfound_exception const&) {
    TLOG(27) << "_importUserData() No document.metadata";
  }

  // replace data if any
  try {
    auto data = document.findChild(jsonliteral::changelog);

    TLOG(28) << "_importUserData() Found converted.changelog=<" << data << ">";

    _document.replaceChild(data, jsonliteral::changelog);

  } catch (notfound_exception const&) {
    TLOG(29) << "_importUserData() No converted.changelog";
  }

  // replace data origin if any
  try {
    auto data = document.findChild(jsonliteral::origin);

    TLOG(30) << "_importUserData() Found origin=<" << data << ">";

    _document.replaceChild(data, jsonliteral::origin);

  } catch (notfound_exception const&) {
    TLOG(31) << "_importUserData() No origin";
  }

  // replace data origin if any
  try {
    auto collection = document.findChild(jsonliteral::collection);

    TLOG(32) << "_importUserData() Found origin=<" << collection << ">";

    _document.replaceChild(collection, jsonliteral::collection);

  } catch (notfound_exception const&) {
    TLOG(33) << "_importUserData() No collection";
  }

  // replace data attachments if any
  try {
    auto attachments = document.findChild(jsonliteral::attachments);

    TLOG(34) << "_importUserData() Found origin=<" << attachments << ">";

    _document.replaceChild(attachments, jsonliteral::attachments);

  } catch (notfound_exception const&) {
    TLOG(35) << "_importUserData() No attachments";
  }

  // replace data if any
  try {
    auto path = ""s + jsonliteral::document + jsonliteral::dot + jsonliteral::data;
    auto data = document.findChild(path);

    TLOG(36) << "_importUserData() Found document.data=<" << data << ">";

    _document.replaceChild(data, path);

    return;
  } catch (notfound_exception const&) {
    TLOG(37) << "_importUserData() No document.data";
  }

  // document contains data only
  try {
    auto path = ""s + jsonliteral::document + jsonliteral::dot + jsonliteral::data;
    _document.replaceChild(document, path);
  } catch (notfound_exception const&) {
    TLOG(38) << "_importUserData() No document.data";
  }

  TLOG(25) << "_importUserData() end";
}

std::string JSONDocument::value(JSONDocument const& document) {
  TLOG(39) << "value() document=<" << document.cached_json_buffer() << ">";

  auto docValue = document.getPayloadValueForKey("null");

  if (type(docValue) == type_t::OBJECT) {
    return JSONDocument{docValue}.to_string();
  }
  { return tostring_visitor(docValue); }
}

std::string JSONDocument::value_at(JSONDocument const& document, std::size_t index) try {
  TLOG(40) << "value_at() begin json=<" << document.cached_json_buffer() << ">";
  TLOG(41) << "value_at() begin index=<" << index << ">";

  auto docValue = document.getPayloadValueForKey("0");

  auto const& valueArray = unwrap(docValue).value_as<const array_t>();

  if (valueArray.empty()) {
    throw runtime_error("JSONDocument") << "Failed calling value_at(): valueArray is empty, document=<" << document.cached_json_buffer() << ">";
  }

  if (valueArray.size() < index) {
    throw runtime_error("JSONDocument") << "Failed to call value_at(); not enough elements, document=<" << document.cached_json_buffer() << ">";
  }

  auto pos = valueArray.begin();

  std::advance(pos, index);

  if (type(*pos) == type_t::OBJECT) {
    return JSONDocument{*pos}.to_string();
  }
  { return tostring_visitor(*pos); }
} catch (std::exception& ex) {
  throw runtime_error("JSONDocument") << "Failed calling value_at(): Caught exception:" << ex.what();
}

value_t& JSONDocument::findChildValue(path_t const& path) try {
  auto const& myslef = self();

  return const_cast<value_t&>(myslef.findChildValue(path));
} catch (std::exception& ex) {
  TLOG(42) << "findChildValue() Search failed; Error:" << ex.what();
  throw;
}

result_t JSONDocumentBuilder::SaveUndo() { return Success(); }

result_t JSONDocumentBuilder::CallUndo() noexcept try { return Success(); } catch (...) {
  return Failure();
}

void dbdr::debug::JSONDocumentUtils() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(43) << "artdaq::database::JSONDocument trace_enable";
}

namespace artdaq {
namespace database {
namespace docrecord {
template <>
JSONDocument toJSONDocument<string_pair_t>(string_pair_t const& pair) {
  std::ostringstream oss;
  oss << '{';
  oss << db::quoted_(pair.first) << ":" << db::quoted_(pair.second);
  oss << '}';

  return {oss.str()};
}
}  // namespace docrecord
}  // namespace database
}  // namespace artdaq
