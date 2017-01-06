#include "artdaq-database/JsonDocument/JSONDocument.h"

#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/Json/json_types_impl.h"
#include "artdaq-database/DataFormats/common/helper_functions.h"

#include "artdaq-database/JsonDocument/common.h"

#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"

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

using namespace artdaq::database::jsonutils;

std::string print_visitor(value_t const&);

void validate(path_t const& path, std::string const& caller) {
  assert(!path.empty());

  if (path.empty())
    throw cet::exception("JSONDocument") << "Failed calling " << caller << "(): Invalid path; path is empty";
}

std::vector<path_t> split_path_validate(path_t const& path, std::string const& caller) {
  auto returnValue = std::vector<path_t>{};

  assert(!caller.empty());

  validate(path, caller);

  auto path_tokens = split_path(path);

  if (path_tokens.empty())
    throw cet::exception("JSONDocument") << "Failed calling " << caller << "(): Invalid path; path=" << path;

  std::reverse(path_tokens.begin(), path_tokens.end());

  std::swap(path_tokens, returnValue);

  return returnValue;
}

value_t const& JSONDocument::findChildValue(path_t const& path) const try {
  TRACE_(5, "findChildValue() begin json_buffer=<" << cached_json_buffer() << ">");
  TRACE_(5, "findChildValue() args  path=<" << path << ">");

  auto path_tokens = split_path_validate(path, "findChildValue");

  auto tmpJson = std::string{};

  std::function<value_t const&(value_t const&, std::size_t)> recurse = [&](value_t const& childValue,
                                                                           std::size_t currentDepth) {
    TRACE_(5, "findChildValue() recurse() args currentDepth=" << currentDepth);

    auto const& path_token = path_tokens.at(currentDepth);

    if (type(childValue) != type_t::OBJECT) {
      throw notfound_exception("JSONDocument")
          << "Failed calling findChildValue(): Search failed for JSON element name=" << path_token
          << ", findChildValue() recurse() value_t is not object_t; value=" << print_visitor(childValue);
    }

    auto const& childDocument = boost::get<object_t>(childValue);

    tmpJson.clear();

    TRACE_(5, "findChildValue() recurse() args currentView=<" << (JsonWriter().write(childDocument, tmpJson), tmpJson)
                                                              << ">");

    if (childDocument.count(path_token) == 0) {
      throw notfound_exception("JSONDocument")
          << "Failed calling findChildValue(): Search failed for JSON element name=" << path_token << ", search path =<"
          << path << ">.";
    }

    auto const& matchedValue = childDocument.at(path_token);

    if (currentDepth == 0) return matchedValue;

    return recurse(matchedValue, currentDepth - 1);
  };

  auto const& found_value = recurse(_value, path_tokens.size() - 1);

  TRACE_(5, "findChildValue() found child value=" << print_visitor(found_value));

  return found_value;
} catch (std::exception& ex) {
  TRACE_(5, "findChildValue() const Search failed; Error:" << ex.what());
  throw;
}

// returns found child
JSONDocument JSONDocument::findChild(path_t const& path) const try {
  TRACE_(6, "findChild() begin json_buffer=<" << cached_json_buffer() << ">");
  TRACE_(6, "findChild() args  path=<" << path << ">");

  validate(path, "findChild");

  auto found_value = findChildValue(path);

  TRACE_(6, "findChild() found child value=" << print_visitor(found_value));

  auto returnValue = JSONDocument();

  auto& document = boost::get<object_t>(returnValue._value);
  document[path] = found_value;

  TRACE_(6, "findChild() resultDocument=<" << returnValue.cached_json_buffer() << ">");
  TRACE_(6, "findChild() Find succeeded.");

  return returnValue;
} catch (std::exception& ex) {
  TRACE_(6, "findChild() Search failed; Error:" << ex.what());
  throw;
}

// returns old child
JSONDocument JSONDocument::replaceChild(JSONDocument const& newChild, path_t const& path) try {
  TRACE_(4, "replaceChild() begin json_buffer=<" << cached_json_buffer() << ">");
  TRACE_(4, "replaceChild() args  newChild=<" << newChild.cached_json_buffer() << ">");
  TRACE_(4, "replaceChild() args  path=<" << path << ">");

  auto path_tokens = split_path_validate(path, "replaceChild");

  auto newValue = newChild.getPayloadValueForKey(path_tokens.at(0));

  TRACE_(4, "replaceChild() new child value=" << print_visitor(newValue));

  auto tmpJson = std::string{};

  std::function<value_t(value_t&, std::size_t)> recurse = [&](value_t& childValue, std::size_t currentDepth) {
    TRACE_(4, "replaceChild() recurse() args currentDepth=" << currentDepth);
    auto const& path_token = path_tokens.at(currentDepth);

    if (currentDepth == 0 && type(childValue) != type_t::OBJECT) {
      throw notfound_exception("JSONDocument")
          << "Failed calling replaceChild(): Replace failed for" << path_token
          << ", replaceChild() recurse() value_t is not object_t; value=" << print_visitor(childValue);
    }

    auto& childDocument = boost::get<object_t>(childValue);

    tmpJson.clear();
    TRACE_(4, "replaceChild() recurse() args childValue=<" << (JsonWriter().write(childDocument, tmpJson), tmpJson)
                                                           << ">");

    if (childDocument.count(path_token) == 0) {
      throw notfound_exception("JSONDocument") << "Failed calling replaceChild(): Replace failed for " << path_token
                                               << ", search path =<" << path << ">.";
    }

    auto& matchedValue = childDocument.at(path_token);

    if (currentDepth == 0) {
      matchedValue.swap(newValue);
      return newValue;
    }

    return recurse(matchedValue, currentDepth - 1);
  };

  auto replaced_value = std::move(recurse(_value, path_tokens.size() - 1));
  writeJson();  // update _cached_json_buffer

  TRACE_(4, "replaceChild() old child value=" << print_visitor(replaced_value));
  TRACE_(4, "replaceChild() resultDocument=<" << cached_json_buffer() << ">");
  TRACE_(4, "replaceChild() Replace succeeded.");

  return JSONDocument(replaced_value);
} catch (std::exception& ex) {
  TRACE_(4, "replaceChild() Replace failed; Error:" << ex.what());
  throw;
}

// returns inserted child
JSONDocument JSONDocument::insertChild(JSONDocument const& newChild, path_t const& path) try {
  TRACE_(2, "insertChild() begin json_buffer=<" << cached_json_buffer() << ">");
  TRACE_(2, "insertChild() args  newChild=<" << newChild.cached_json_buffer() << ">");
  TRACE_(2, "insertChild() args  path=<" << path << ">");

  auto path_tokens = split_path_validate(path, "insertChild");

  auto const& newValue = newChild.getPayloadValueForKey(path_tokens.at(0));

  TRACE_(2, "insertChild() new child value=" << print_visitor(newValue));

  auto tmpJson = std::string{};

  std::function<value_t(value_t&, std::size_t)> recurse = [&](value_t& childValue, std::size_t currentDepth) {
    TRACE_(2, "insertChild() recurse() args currentDepth=" << currentDepth);

    auto const& path_token = path_tokens.at(currentDepth);

    if (currentDepth == 0 && type(childValue) != type_t::OBJECT) {
      throw notfound_exception("JSONDocument")
          << "Failed calling insertChild(): Insert failed for" << path_token
          << ", insertChild() recurse() value_t is not object_t; value=" << print_visitor(childValue);
    }

    auto& childDocument = boost::get<object_t>(childValue);

    tmpJson.clear();
    TRACE_(2, "insertChild() recurse() args childValue=<" << (JsonWriter().write(childDocument, tmpJson), tmpJson)
                                                          << ">");

    auto numberOfChildren = childDocument.count(path_token);

    if (currentDepth == 0 && numberOfChildren != 0) {
      throw notfound_exception("JSONDocument") << "Failed calling insertChild(): Insert failed for" << path_token
                                               << ", search path =<" << path << ">; Child exists, call replace instead";

    } else if (currentDepth != 0 && numberOfChildren == 0) {
      throw notfound_exception("JSONDocument") << "Failed calling insertChild(): Insert failed for" << path_token
                                               << ", search path =<" << path << ">.";
    } else if (currentDepth == 0 && numberOfChildren == 0) {
      auto const& returnValue = childDocument[path_token] = newValue;
      return returnValue;
    }

    auto& matchedValue = childDocument.at(path_token);

    return recurse(matchedValue, currentDepth - 1);
  };

  auto inserted_value = std::move(recurse(_value, path_tokens.size() - 1));
  writeJson();  // update _cached_json_buffer

  TRACE_(2, "insertChild() resultDocument=<" << cached_json_buffer() << ">");
  TRACE_(2, "insertChild() Insert succeeded.");

  return JSONDocument(inserted_value);
} catch (std::exception& ex) {
  TRACE_(2, "insertChild() Insert failed; Error:" << ex.what());
  throw;
}

// returns old child
JSONDocument JSONDocument::deleteChild(path_t const& path) try {
  TRACE_(3, "deleteChild() begin json_buffer=<" << cached_json_buffer() << ">");
  TRACE_(3, "deleteChild() args  path=<" << path << ">");

  auto path_tokens = split_path_validate(path, "deleteChild");

  auto tmpJson = std::string{};

  std::function<value_t(value_t&, std::size_t)> recurse = [&](value_t& childValue, std::size_t currentDepth) {
    TRACE_(3, "deleteChild() recurse() args currentDepth=" << currentDepth);

    auto const& path_token = path_tokens.at(currentDepth);

    if (currentDepth == 0 && type(childValue) != type_t::OBJECT) {
      throw notfound_exception("JSONDocument") << "Failed calling deleteChild(): Delete failed for " << path_token
                                               << ", deleteChild() recurse() value_t is not object_t;  "
                                                  "value="
                                               << print_visitor(childValue);
    }

    auto& childDocument = boost::get<object_t>(childValue);

    tmpJson.clear();
    TRACE_(3, "deleteChild() recurse() args childValue=<" << (JsonWriter().write(childDocument, tmpJson), tmpJson)
                                                          << ">");

    if (childDocument.count(path_token) == 0) {
      throw notfound_exception("JSONDocument") << "Failed calling deleteChild(): Delete failed for " << path_token
                                               << ", search path =<" << path << ">.";
    }

    auto& matchedValue = childDocument.at(path_token);

    if (currentDepth == 0) {
      return childDocument.delete_at(path_token);
    }

    return recurse(matchedValue, currentDepth - 1);
  };

  auto deleted_value = recurse(_value, path_tokens.size() - 1);
  writeJson();  // update _cached_json_buffer

  TRACE_(3, "deleteChild() deleted child value=" << print_visitor(deleted_value));
  TRACE_(3, "deleteChild() resultDocument=<" << cached_json_buffer() << ">");
  TRACE_(3, "deleteChild() Insert succeeded.");
  //    if (path == "_id") std::replace(return_json.begin(), return_json.end(), '$', '_');

  return JSONDocument(deleted_value);
} catch (std::exception& ex) {
  TRACE_(3, "deleteChild() Delete failed; Error:" << ex.what());
  throw;
}

// returns added child
JSONDocument JSONDocument::appendChild(JSONDocument const& newChild, path_t const& path) try {
  TRACE_(7, "appendChild() begin json_buffer=<" << cached_json_buffer() << ">");
  TRACE_(7, "appendChild() args  path=<" << path << ">");

  validate(path, "appendChild");

  auto const& newValue = newChild._value;

  TRACE_(4, "appendChild() new child value=" << print_visitor(newValue));

  auto& appendParent = boost::get<object_t>(findChildValue(path));

  auto firstResultIt = appendParent.begin();

  if (firstResultIt == appendParent.end() || appendParent.size() != 1 || type(firstResultIt->value) != type_t::ARRAY) {
    throw cet::exception("JSONDocument") << "Failed calling appendChild(): Target path is "
                                         << "not an array_t element, path=<" << path << ">";
  }

  boost::get<array_t>(firstResultIt->value).push_back(newValue);
  writeJson();  // update _cached_json_buffer

  TRACE_(3, "appendChild() resultDocument=<" << cached_json_buffer() << ">");
  TRACE_(3, "appendChild() Append succeeded.");

  return {newValue};
} catch (std::exception& ex) {
  TRACE_(7, "appendChild() Append failed; Error:" << ex.what());
  throw;
}

bool matches(value_t const&, value_t const&);
// returns removed child
JSONDocument JSONDocument::removeChild(JSONDocument const& delChild, path_t const& path) try {
  TRACE_(7, "removeChild() begin json_buffer=<" << cached_json_buffer() << ">");
  TRACE_(7, "removeChild() args  path=<" << path << ">");

  validate(path, "removeChild");

  auto const& delValue = delChild._value;

  TRACE_(7, "removeChild() delete value=" << print_visitor(delValue));

  auto& removeParent = boost::get<object_t>(findChildValue(path));

  auto firstResultIt = removeParent.begin();

  if (firstResultIt == removeParent.end() || removeParent.size() != 1 || type(firstResultIt->value) != type_t::ARRAY) {
    throw cet::exception("JSONDocument") << "Failed calling removeChild(): Target path is "
                                         << "not an array_t element, path=<" << path << ">";
  }

  auto& deleteCandidates = boost::get<array_t>(firstResultIt->value);
  auto const& deleteValue = delChild.getPayloadValueForKey("null");

  auto returnValue = JSONDocument();

  returnValue._value = array_t{};

  auto deletedValues = boost::get<array_t>(returnValue._value);

  auto candidateIter(deleteCandidates.begin()), end(deleteCandidates.end());
  for (; candidateIter != end; candidateIter++) {
    if (matches(deleteValue, *candidateIter)) {
      deletedValues.push_back(*candidateIter);
      deleteCandidates.erase(candidateIter);
    }
  }

  writeJson();              // update _cached_json_buffer
  returnValue.writeJson();  // update _cached_json_buffer

  TRACE_(7, "removeChild() resultDocument=<" << cached_json_buffer() << ">");
  TRACE_(7, "removeChild() removedChildren=<" << returnValue.cached_json_buffer() << ">");
  TRACE_(7, "removeChild() Remove succeeded.");

  return returnValue;

} catch (std::exception& ex) {
  TRACE_(7, "removeChild() Remove failed; Error:" << ex.what());
  throw;
}

void debug::enableJSONDocument() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::JSONDocument trace_enable");
}
