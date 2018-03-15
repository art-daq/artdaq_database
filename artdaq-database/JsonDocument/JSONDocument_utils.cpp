#include "artdaq-database/BasicTypes/data_json.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/common.h"

#include <boost/filesystem.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSNU:DocUtils_C"

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::json::array_t;
using artdaq::database::json::type_t;

using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;

using artdaq::database::Success;
using artdaq::database::Failure;
using artdaq::database::result_t;

using namespace artdaq::database::docrecord;

namespace db = artdaq::database;
namespace utl = db::docrecord;

namespace dbdr = artdaq::database::docrecord;

namespace jsonliteral = artdaq::database::dataformats::literal;

std::string print_visitor(value_t const& value) { return boost::apply_visitor(jsn::print_visitor(), value); }
std::string tostring_visitor(value_t const& value) { return boost::apply_visitor(jsn::tostring_visitor(), value); }

bool matches(value_t const& left, value_t const& right) {
  if (left.type() != right.type()) return false;

  if (type(left) == type_t::OBJECT) {
    auto const& leftObj = boost::get<object_t>(left);
    auto const& rightObj = boost::get<object_t>(right);

    // FIXME:GAL partial elements match
    // if (leftObj.size() != rightObj.size()) return false;

    if (leftObj.empty() || rightObj.empty()) return false;

    auto const& tempateObj = leftObj.size() >= rightObj.size() ? leftObj : rightObj;
    auto const& candidateObj = leftObj.size() >= rightObj.size() ? rightObj : leftObj;

    for (auto const& templateKVP : tempateObj) {
      if (candidateObj.count(templateKVP.key) != 1) continue;

      auto const& candidateVal = candidateObj.at(templateKVP.key);
      auto const& templateVal = templateKVP.value;

      if (!matches(templateVal, candidateVal)) {
        TLOG(11) << "matches() objects are different at key=<" << templateKVP.key << ">";

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

  std::replace(tmp.begin(), tmp.end(), '.', ' ');

  std::istringstream iss(tmp);

  std::vector<std::string> tokens{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

  if (!tokens.empty()) {
    std::ostringstream oss;
    for (auto const& token : tokens) oss << "\"" << token << "\",";
  }

  return tokens;
}

std::ostream& utl::operator<<(std::ostream& os, JSONDocument const& document) {
  os << document.to_string();

  return os;
}

value_t JSONDocument::readJson(std::string const& json) {
  if (json.empty()) throw invalid_argument("JSONDocument") << "Failed reading JSON: Empty JSON buffer";

  auto tmpObject = object_t{};

  if (JsonReader().read(json, tmpObject)) return {tmpObject};

  throw invalid_argument("JSONDocument") << "Failed reading JSON: Invalid json; json_buffer=" << json;
}

std::string JSONDocument::writeJson() const {
  if (type(_value) != type_t::OBJECT)
    throw invalid_argument("JSONDocument") << "Failed writing JSON: Wrong value type: type(_value) != type_t::OBJECT";

  auto const& tmpObject = boost::get<object_t>(_value);

  if (tmpObject.empty()) return "{}";

  auto json = std::string{};

  if (!JsonWriter().write(tmpObject, json))
    throw invalid_argument("JSONDocument") << "Failed writing JSON: JSONDocument::_value has invalid AST";

  // TLOG(11) << "writeJson() json=<" << json << " > ");

  return json;
}

std::string JSONDocument::to_string() const { return writeJson(); }

JSONDocument::JSONDocument(JsonData const& data) : _value{readJson(data)}, _cached_json_buffer(data) {}

JSONDocument::JSONDocument(std::string const& json) : _value{readJson(json)}, _cached_json_buffer(json) {}

JSONDocument::JSONDocument(value_t const& value) : _value{value}, _cached_json_buffer(writeJson()) {}

JSONDocument::JSONDocument() : _value{object_t{}}, _cached_json_buffer(writeJson()) {}

std::string const& JSONDocument::cached_json_buffer() const { return _cached_json_buffer; }

void JSONDocument::update_json_buffer() { _cached_json_buffer = writeJson(); }

value_t const& JSONDocument::getPayloadValueForKey(object_t::key_type const& key) const {
  confirm(!key.empty());

  TLOG(25)<< "getPayloadValueForKey() document=<" << cached_json_buffer() << ">";

  if (boost::get<object_t>(_value).count("payload") == 1) {
    auto const& value = boost::get<object_t>(_value).at("payload");

    if (type(value) == type_t::OBJECT && boost::get<object_t>(value).count(key) == 1)
      return boost::get<object_t>(value).at(key);
    else
      return value;
  } else if (boost::get<object_t>(_value).size() == 1) {
    return boost::get<object_t>(_value).begin()->value;
  }

  return _value;
}

bool JSONDocument::equals(JSONDocument const& other) const {
  auto result = jsn::operator==(_value, other._value);

  TLOG(20)<< "matches() JSON buffers are " << (result.first ? "equal." : "not equal.");

  if (result.first) return true;

  TLOG(20)<< "matches() Error message=<" << result.second << ">";

  return false;
}

JSONDocument JSONDocument::loadFromFile(std::string const& fileName) try {
  auto json_buffer = std::string{};

  if (!db::read_buffer_from_file(json_buffer, fileName))
    throw invalid_argument("JSONDocument") << "Failed calling loadFromFile(): Failed opening a JSON file=" << fileName;

  return {json_buffer};
} catch (std::exception& ex) {
  throw runtime_error("JSONDocument") << "Failed calling loadFromFile(): Caught exception:" << ex.what();
}

bool JSONDocument::saveToFile(std::string const& fileName) try {
  if (fileName.empty()) throw invalid_argument("JSONDocument") << "Failed calling saveToFile(): File name is empty.";

  if (boost::filesystem::exists(fileName)) {
    boost::filesystem::copy_file(fileName, std::string{fileName} + ".bak",
                                 boost::filesystem::copy_option::overwrite_if_exists);
  }

  auto buffer = to_string();

  return db::write_buffer_to_file(buffer, fileName);
} catch (std::exception& ex) {
  throw runtime_error("JSONDocument") << "Failed calling saveToFile(): Caught exception:" << ex.what();
}

JSONDocumentBuilder& JSONDocumentBuilder::createFromData(JSONDocument const& document) {
  _overlay.reset(nullptr);

  _createFromTemplate(JSONDocument(std::string(template__empty_document)));

  {  // create a new document template using overlay classes
    auto ovl = std::make_unique<ovlDatabaseRecord>(_document._value);
    std::swap(_overlay, ovl);
  }

  TLOG(12) << "createFrom() args  document=<" << document << ">";

  _importUserData(document);

  {  // refresh overlays
    auto ovl = std::make_unique<ovlDatabaseRecord>(_document._value);
    std::swap(_overlay, ovl);
  }
  _document.writeJson();

  TLOG(12) << "createFrom() imported document=<" << _document.cached_json_buffer() << ">";

  return self();
}

void JSONDocumentBuilder::_importUserData(JSONDocument const& document) {
  // replace metadata if any
  try {
    auto path = ""s + jsonliteral::document + jsonliteral::dot + jsonliteral::metadata;
    auto metadata = document.findChild(path);

    TLOG(12) << "_importUserData() Found document.metadata=<" << metadata << ">";

    _document.replaceChild(metadata, path);

  } catch (notfound_exception const&) {
    TLOG(12) << "_importUserData() No document.metadata";
  }

  // replace data if any
  try {
    auto data = document.findChild(jsonliteral::changelog);

    TLOG(12) << "_importUserData() Found converted.changelog=<" << data << ">";

    _document.replaceChild(data, jsonliteral::changelog);

  } catch (notfound_exception const&) {
    TLOG(12) << "_importUserData() No converted.changelog";
  }

  // replace data origin if any
  try {
    auto data = document.findChild(jsonliteral::origin);

    TLOG(12) << "_importUserData() Found origin=<" << data << ">";

    _document.replaceChild(data, jsonliteral::origin);

  } catch (notfound_exception const&) {
    TLOG(12) << "_importUserData() No origin";
  }

  // replace data origin if any
  try {
    auto collection = document.findChild(jsonliteral::collection);

    TLOG(12) << "_importUserData() Found origin=<" << collection << ">";

    _document.replaceChild(collection, jsonliteral::collection);

  } catch (notfound_exception const&) {
    TLOG(12) << "_importUserData() No collection";
  }

  // replace data attachments if any
  try {
    auto attachments = document.findChild(jsonliteral::attachments);

    TLOG(12) << "_importUserData() Found origin=<" << attachments << ">";

    _document.replaceChild(attachments, jsonliteral::attachments);

  } catch (notfound_exception const&) {
    TLOG(12) << "_importUserData() No attachments";
  }

  // replace data if any
  try {
    auto path = ""s + jsonliteral::document + jsonliteral::dot + jsonliteral::data;
    auto data = document.findChild(path);

    TLOG(12) << "_importUserData() Found document.data=<" << data << ">";

    _document.replaceChild(data, path);

    return;
  } catch (notfound_exception const&) {
    TLOG(12) << "_importUserData() No document.data";
  }

  // document contains data only
  try {
    auto path = ""s + jsonliteral::document + jsonliteral::dot + jsonliteral::data;
    _document.replaceChild(document, path);
  } catch (notfound_exception const&) {
    TLOG(12) << "_importUserData() No document.data";
  }
}

std::string JSONDocument::value(JSONDocument const& document) {
  TLOG(24) << "value() document=<" << document.cached_json_buffer() << ">";

  auto docValue = document.getPayloadValueForKey("null");

  if (type(docValue) == type_t::OBJECT)
    return JSONDocument{docValue}.to_string();
  else
    return tostring_visitor(docValue);
}

std::string JSONDocument::value_at(JSONDocument const& document, std::size_t index) try {
  TLOG(24) << "value_at() begin json=<" << document.cached_json_buffer() << ">";
  TLOG(24) << "value_at() begin index=<" << index << ">";

  auto docValue = document.getPayloadValueForKey("0");


  auto const& valueArray = boost::get<jsn::array_t>(docValue);

  if (valueArray.empty())
    throw runtime_error("JSONDocument") << "Failed calling value_at(): valueArray is empty, document=<"
                                        << document.cached_json_buffer() << ">";

  if (valueArray.size() < index)
    throw runtime_error("JSONDocument") << "Failed to call value_at(); not enough elements, document=<"
                                        << document.cached_json_buffer() << ">";

  auto pos = valueArray.begin();

  std::advance(pos, index);

  if (type(*pos) == type_t::OBJECT)
    return JSONDocument{*pos}.to_string();
  else
    return tostring_visitor(*pos);
} catch (std::exception& ex) {
  throw runtime_error("JSONDocument") << "Failed calling value_at(): Caught exception:" << ex.what();
}

value_t& JSONDocument::findChildValue(path_t const& path) try {
  auto const& myslef = self();

  return const_cast<value_t&>(myslef.findChildValue(path));
} catch (std::exception& ex) {
  TLOG(15) << "findChildValue() Search failed; Error:" << ex.what();
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

  TLOG(10) <<  "artdaq::database::JSONDocument trace_enable";
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
}
}
}