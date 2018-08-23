#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/DataFormats/Json/json_types_impl.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSONDocument.cpp"

using artdaq::database::json::array_t;
using artdaq::database::json::object_t;
using artdaq::database::json::type_t;
using artdaq::database::json::value_t;

using namespace artdaq::database;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::split_path;

namespace dbdr = artdaq::database::docrecord;

std::string print_visitor(value_t const&);

void validate(path_t const& path, std::string const& caller) {
  if (path.empty()) {
    throw invalid_argument("JSONDocument") << "Failed calling " << caller << "(): Invalid path; path is empty";
  }
}

std::vector<path_t> split_path_validate(path_t const& path, std::string const& caller) {
  auto returnValue = std::vector<path_t>{};

  confirm(!caller.empty());

  validate(path, caller);

  auto path_tokens = split_path(path);

  if (path_tokens.empty()) {
    throw invalid_argument("JSONDocument") << "Failed calling " << caller << "(): Invalid path; path=" << path;
  }

  std::reverse(path_tokens.begin(), path_tokens.end());

  std::swap(path_tokens, returnValue);

  return returnValue;
}

value_t const& JSONDocument::findChildValue(path_t const& path) const try {
  TLOG(20) << "findChildValue() args  path=<" << path << ">";

  auto path_tokens = split_path_validate(path, "findChildValue");

  auto tmpJson = std::string{};

  std::function<value_t const&(value_t const&, std::size_t)> recurse = [&](value_t const& childValue,
                                                                           std::size_t currentDepth) -> value_t const& {
    TLOG(21) << "findChildValue() recurse() args currentDepth=" << currentDepth;

    auto const& path_token = path_tokens.at(currentDepth);

    if (type(childValue) != type_t::OBJECT) {
      throw notfound_exception("JSONDocument")
          << "Failed calling findChildValue(): Search failed for JSON element name=" << path_token
          << ", findChildValue() recurse() value_t is not object_t; value=" << print_visitor(childValue);
    }

    auto const& childDocument = boost::get<object_t>(childValue);

    tmpJson.clear();

    if (childDocument.count(path_token) == 0) {
      throw notfound_exception("JSONDocument")
          << "Failed calling findChildValue(): Search failed for JSON element name=" << path_token << ", search path =<"
          << path << ">.";
    }

    auto const& matchedValue = childDocument.at(path_token);

    if (currentDepth == 0) {
      return matchedValue;
    }

    return recurse(matchedValue, currentDepth - 1);
  };

  auto const& found_value = recurse(_value, path_tokens.size() - 1);

  TLOG(22) << "findChildValue() found child value=" << print_visitor(found_value);

  return found_value;
} catch (std::exception& ex) {
  TLOG(23) << "findChildValue() const Search failed; Error:" << ex.what();
  throw;
}

// returns found child
JSONDocument JSONDocument::findChild(path_t const& path) const try {
  TLOG(24) << "findChild() args  path=<" << path << ">";

  validate(path, "findChild");

  auto const& found_value = findChildValue(path);

  TLOG(25) << "findChild() found child value=" << print_visitor(found_value);

  auto returnValue = JSONDocument();

  auto& document = boost::get<object_t>(returnValue._value);
  document[path] = found_value;

  TLOG(26) << "findChild() Find succeeded.";

  return returnValue;
} catch (std::exception& ex) {
  TLOG(27) << "findChild() Search failed; Error:" << ex.what();
  throw;
}

// returns found child value as a document
JSONDocument JSONDocument::findChildDocument(path_t const& path) const try {
  TLOG(28) << "findChildDocument() args  path=<" << path << ">";

  validate(path, "findChildDocument");

  auto const& found_value = findChildValue(path);

  if (type(found_value) != type_t::OBJECT) {
    throw notfound_exception("JSONDocument") << "Failed calling findChildDocument(): Search failed for" << path
                                             << ", value_t is not object_t; value=" << print_visitor(found_value);
  }

  auto returnValue = JSONDocument(found_value);

  TLOG(29) << "findChildDocument() Find succeeded.";

  return returnValue;
} catch (std::exception& ex) {
  TLOG(30) << "findChildDocument() Search failed; Error:" << ex.what();
  throw;
}

// returns old child
JSONDocument JSONDocument::replaceChild(JSONDocument const& newChild, path_t const& path) try {
  TLOG(31) << "replaceChild() args  path=<" << path << ">";

  auto path_tokens = split_path_validate(path, "replaceChild");

  auto newValue = newChild.getPayloadValueForKey(path_tokens.at(0));

  TLOG(32) << "replaceChild() new child value=" << print_visitor(newValue);

  auto tmpJson = std::string{};

  std::function<value_t(value_t&, std::size_t)> recurse = [&](value_t& childValue,
                                                              std::size_t currentDepth) -> value_t {
    TLOG(33) << "replaceChild() recurse() args currentDepth=" << currentDepth;
    auto const& path_token = path_tokens.at(currentDepth);

    if (currentDepth == 0 && type(childValue) != type_t::OBJECT) {
      throw notfound_exception("JSONDocument")
          << "Failed calling replaceChild(): Replace failed for" << path_token
          << ", replaceChild() recurse() value_t is not object_t; value=" << print_visitor(childValue);
    }

    auto& childDocument = boost::get<object_t>(childValue);

    tmpJson.clear();

    if (childDocument.count(path_token) == 0) {
      throw notfound_exception("JSONDocument")
          << "Failed calling replaceChild(): Replace failed for " << path_token << ", search path =<" << path << ">.";
    }

    auto& matchedValue = childDocument.at(path_token);

    if (currentDepth == 0) {
      matchedValue.swap(newValue);
      return newValue;
    }

    return recurse(matchedValue, currentDepth - 1);
  };

  auto replaced_value = recurse(_value, path_tokens.size() - 1);
  
  _isDirty=true;

  TLOG(34) << "replaceChild() Replace succeeded.";

  auto obj = object_t{};
  obj[path] = replaced_value;
  value_t returnValue = obj;

  return JSONDocument(returnValue);
} catch (std::exception& ex) {
  TLOG(35) << "replaceChild() Replace failed; Error:" << ex.what();
  throw;
}

// returns inserted child
JSONDocument JSONDocument::insertChild(JSONDocument const& newChild, path_t const& path) try {
  TLOG(36) << "insertChild() args  path=<" << path << ">";

  auto path_tokens = split_path_validate(path, "insertChild");

  auto const& newValue = newChild.getPayloadValueForKey(path_tokens.at(0));

  TLOG(37) << "insertChild() new child value=" << print_visitor(newValue);

  auto tmpJson = std::string{};

  std::function<value_t(value_t&, std::size_t)> recurse = [&](value_t& childValue,
                                                              std::size_t currentDepth) -> value_t {
    TLOG(38) << "insertChild() recurse() args currentDepth=" << currentDepth;

    auto const& path_token = path_tokens.at(currentDepth);

    if (currentDepth == 0 && type(childValue) != type_t::OBJECT) {
      throw notfound_exception("JSONDocument")
          << "Failed calling insertChild(): Insert failed for" << path_token
          << ", insertChild() recurse() value_t is not object_t; value=" << print_visitor(childValue);
    }

    auto& childDocument = boost::get<object_t>(childValue);

    tmpJson.clear();

    auto numberOfChildren = childDocument.count(path_token);

    if (currentDepth == 0 && numberOfChildren != 0) {
      throw notfound_exception("JSONDocument") << "Failed calling insertChild(): Insert failed for" << path_token
                                               << ", search path =<" << path << ">; Child exists, call replace instead";

    } else if (currentDepth != 0 && numberOfChildren == 0) {
      throw notfound_exception("JSONDocument")
          << "Failed calling insertChild(): Insert failed for" << path_token << ", search path =<" << path << ">.";
    } else if (currentDepth == 0 && numberOfChildren == 0) {
      auto const& returnValue = childDocument[path_token] = newValue;
      return returnValue;
    }

    auto& matchedValue = childDocument.at(path_token);

    return recurse(matchedValue, currentDepth - 1);
  };

  auto inserted_value = recurse(_value, path_tokens.size() - 1);
  
  _isDirty=true;

  TLOG(39) << "insertChild() Insert succeeded.";

  return JSONDocument(inserted_value);
} catch (std::exception& ex) {
  TLOG(40) << "insertChild() Insert failed; Error:" << ex.what();
  throw;
}

// returns old child
JSONDocument JSONDocument::deleteChild(path_t const& path) try {
  TLOG(41) << "deleteChild() begin json_buffer=<" << cached_json_buffer() << ">";
  TLOG(42) << "deleteChild() args  path=<" << path << ">";

  auto path_tokens = split_path_validate(path, "deleteChild");

  auto tmpJson = std::string{};

  std::function<value_t(value_t&, std::size_t)> recurse = [&](value_t& childValue,
                                                              std::size_t currentDepth) -> value_t {
    TLOG(43) << "deleteChild() recurse() args currentDepth=" << currentDepth;

    auto const& path_token = path_tokens.at(currentDepth);

    if (currentDepth == 0 && type(childValue) != type_t::OBJECT) {
      throw notfound_exception("JSONDocument") << "Failed calling deleteChild(): Delete failed for " << path_token
                                               << ", deleteChild() recurse() value_t is not object_t;  "
                                                  "value="
                                               << print_visitor(childValue);
    }

    auto& childDocument = boost::get<object_t>(childValue);

    tmpJson.clear();

    if (childDocument.count(path_token) == 0) {
      throw notfound_exception("JSONDocument")
          << "Failed calling deleteChild(): Delete failed for " << path_token << ", search path =<" << path << ">.";
    }

    auto& matchedValue = childDocument.at(path_token);

    if (currentDepth == 0) {
      return childDocument.delete_at(path_token);
    }

    return recurse(matchedValue, currentDepth - 1);
  };

  auto deleted_value = recurse(_value, path_tokens.size() - 1);
  
  _isDirty=true;

  TLOG(44) << "deleteChild() Delete succeeded.";

  return JSONDocument(deleted_value);
} catch (std::exception& ex) {
  TLOG(45) << "deleteChild() Delete failed; Error:" << ex.what();
  throw;
}

// returns added child
JSONDocument JSONDocument::appendChild(JSONDocument const& newChild, path_t const& path) try {
  TLOG(46) << "appendChild() args  path=<" << path << ">";

  auto const& newValue = newChild.getPayloadValueForKey("null");

  TLOG(47) << "appendChild() new child value=" << print_visitor(newValue);

  auto& valueArray = boost::get<array_t>(findChildValue(path));

  valueArray.push_back(newValue);

  _isDirty=true;

  TLOG(48) << "appendChild() Append succeeded.";

  return {newValue};
} catch (std::exception& ex) {
  TLOG(49) << "appendChild() Append failed; Error:" << ex.what();
  throw;
}

bool matches(value_t const&, value_t const&);
// returns removed child
JSONDocument JSONDocument::removeChild(JSONDocument const& delChild, path_t const& path) try {
  TLOG(50) << "removeChild() args  path=<" << path << ">";

  validate(path, "removeChild");

  auto const& deleteValue = delChild.getPayloadValueForKey("null");

  TLOG(51) << "removeChild() delete value=" << print_visitor(deleteValue);

  auto& deletionCandidates = boost::get<array_t>(findChildValue(path));

  auto returnValue = JSONDocument();

  auto& deletedVariant = boost::get<object_t>(returnValue._value)["0"] = array_t{};

  auto& deletedValues = boost::get<array_t>(deletedVariant);

  auto candidateIter(deletionCandidates.begin()), end(deletionCandidates.end());
  while (candidateIter != end) {
    if (matches(deleteValue, *candidateIter)) {
      deletedValues.push_back(*candidateIter);
      candidateIter = deletionCandidates.erase(candidateIter);
    } else {
      candidateIter++;
    }
  }

  _isDirty=true;
  
  TLOG(53) << "removeChild() Remove succeeded.";

  return returnValue;

} catch (std::exception& ex) {
  TLOG(54) << "removeChild() Remove failed; Error:" << ex.what();
  throw;
}

void dbdr::debug::JSONDocument() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(55) << "artdaq::database::JSONDocument trace_enable";
}
