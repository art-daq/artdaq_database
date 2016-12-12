#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/common.h"

#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSNU:Document_C"

namespace artdaq {
namespace database {
namespace jsonutils {

bool matches(jsn::value_t const& left, jsn::value_t const& right) {
  if (left.type() != right.type()) return false;

  if (left.type() == typeid(const jsn::object_t)) {
    auto const& leftObj = boost::get<jsn::object_t>(left);
    auto const& rightObj = boost::get<jsn::object_t>(right);

    // partial elements match
    // if (leftObj.size()!=rightObj.size()) return false;

    if (leftObj.empty() || rightObj.empty()) return false;

    auto const& tempateObj = leftObj.size() >= rightObj.size() ? leftObj : rightObj;
    auto const& candidateObj = leftObj.size() >= rightObj.size() ? rightObj : leftObj;

    for (auto const& templateKVP : tempateObj) {
      if (candidateObj.count(templateKVP.key) != 1) continue;

      auto const& candidateVal = candidateObj.at(templateKVP.key);
      auto const& templateVal = templateKVP.value;

      if (!matches(templateVal, candidateVal)) {
        TRACE_(26,
               "artdaq::database::jsonutils::matches() objects are "
               "different at key=<"
                   << templateKVP.key << ">");

        return false;
      }
    }

    return true;

  } else if (left.type() == typeid(const jsn::array_t)) {
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
    auto leftObj = boost::apply_visitor(jsn::tostring_visitor(), left);
    auto rightObj = boost::apply_visitor(jsn::tostring_visitor(), right);

    return leftObj == rightObj;
  }
}

// returns found child
JSONDocument JSONDocument::findChild(path_t const& path) const {
  assert(!path.empty());

  TRACE_(5, "findChild() begin _json_buffer=<" << _json_buffer << ">");
  TRACE_(5, "findChild() args  path=<" << path << ">");

  auto path_tokens = _split_path(path);

  if (path_tokens.empty()) throw cet::exception("JSONDocument") << "Invalid paht; path=" << path;

  std::reverse(path_tokens.begin(), path_tokens.end());

  auto document = jsn::value_t{jsn::object_t{}};

  if (!jsn::JsonReader().read(_json_buffer, boost::get<jsn::object_t>(document))) {
    TRACE_(5, "findChild()"
                  << " Invalid initialDocument");

    throw cet::exception("JSONDocument") << " Invalid initialDocument; json_buffer=" << _json_buffer;
  }

  auto tmpJson = std::string{};

  std::function<jsn::value_t const(jsn::value_t const&, std::size_t)> recurse = [&](jsn::value_t const& childValue,
                                                                                    std::size_t currentDepth) {
    TRACE_(5, "findChild() recurse() args currentDepth=" << currentDepth);

    if (childValue.type() != typeid(const jsn::object_t)) {
      TRACE_(5, "findChild() recurse() jsn::value_t is not jsn::object_t;  value=" << boost::apply_visitor(
                    jsn::print_visitor(), childValue));

      throw notfound_exception("JSONDocument") << "Search failed for json element name=" << path_tokens.at(currentDepth)
                                               << ", findChild() recurse() jsn::value_t is not jsn::object_t;  "
                                                  "value="
                                               << boost::apply_visitor(jsn::print_visitor(), childValue);
    }

    auto const& childDocument = boost::get<jsn::object_t>(childValue);

    tmpJson.clear();
    TRACE_(5, "findChild() recurse() args currentView=<" << (jsn::JsonWriter().write(childDocument, tmpJson), tmpJson)
                                                         << ">");

    if (childDocument.count(path_tokens.at(currentDepth)) == 0) {
      TRACE_(5,
             "findChild() recurse() Error: Search failed for json "
             "element name="
                 << path_tokens.at(currentDepth) << ", search path =<" << path << ">.");

      throw notfound_exception("JSONDocument") << "Search failed for json element name=" << path_tokens.at(currentDepth)
                                               << ", search path =<" << path << ">.";
    }

    auto const& matchedValue = childDocument.at(path_tokens.at(currentDepth));

    if (currentDepth == 0) return matchedValue;

    return recurse(matchedValue, currentDepth - 1);
  };

  auto found_value = recurse(document, path_tokens.size() - 1);

  TRACE_(5, "findChild() found child value=" << boost::apply_visitor(jsn::print_visitor(), found_value));

  auto result = jsn::object_t{};

  result[path] = jsn::value_t(found_value);

  tmpJson.clear();

  jsn::JsonWriter().write(result, tmpJson);

  TRACE_(5, "findChild() resultDocument=<" << tmpJson << ">");

  TRACE_(5, "findChild() Find succeeded.");

  return {tmpJson};
}

// returns old child
JSONDocument JSONDocument::replaceChild(JSONDocument const& newChild, path_t const& path) {
  assert(!path.empty());
  assert(!newChild._json_buffer.empty());

  TRACE_(4, "replaceChild() begin _json_buffer=<" << _json_buffer << ">");
  TRACE_(4, "replaceChild() args  newChild=<" << newChild._json_buffer << ">");
  TRACE_(4, "replaceChild() args  path=<" << path << ">");

  auto path_tokens = _split_path(path);

  if (path_tokens.empty()) throw cet::exception("JSONDocument") << "Invalid path; path=" << path;

  std::reverse(path_tokens.begin(), path_tokens.end());

  auto document = jsn::value_t{jsn::object_t{}};

  if (!jsn::JsonReader().read(_json_buffer, boost::get<jsn::object_t>(document))) {
    TRACE_(4, "replaceChild()"
                  << " Invalid initialDocument");

    throw cet::exception("JSONDocument") << " Invalid initialDocument; json_buffer=" << _json_buffer;
  }

  auto tmpValue = jsn::value_t{jsn::object_t{}};

  if (!jsn::JsonReader().read(newChild._json_buffer, boost::get<jsn::object_t>(tmpValue))) {
    TRACE_(4, "replaceChild()"
                  << " Invalid newChild document");

    throw cet::exception("JSONDocument") << " Invalid newChild document; json_buffer=" << newChild._json_buffer;
  }

  auto newValue = jsn::value_t{};

  if (boost::get<jsn::object_t>(tmpValue).count("payload") == 1) {
    newValue = boost::get<jsn::object_t>(tmpValue).at("payload");
    if (newValue.type() == typeid(jsn::object_t) && boost::get<jsn::object_t>(newValue).count(path_tokens.at(0)) == 1)
      newValue = jsn::value_t{boost::get<jsn::object_t>(newValue).at(path_tokens.at(0))};
  } else if (boost::get<jsn::object_t>(tmpValue).size() == 1) {
    newValue = boost::get<jsn::object_t>(tmpValue).begin()->value;
  } else {
    newValue = tmpValue;
    TRACE_(4, "replaceChild()"
                  << " Replacement payload <" << newChild._json_buffer << ">");
  }

  TRACE_(4, "replaceChild() new child value=" << boost::apply_visitor(jsn::print_visitor(), newValue));

  auto tmpJson = std::string{};

  std::function<jsn::value_t(jsn::value_t&, std::size_t)> recurse = [&](jsn::value_t& childValue,
                                                                        std::size_t currentDepth) {
    TRACE_(4, "replaceChild() recurse() args currentDepth=" << currentDepth);

    if (currentDepth == 0 && childValue.type() != typeid(jsn::object_t)) {
      TRACE_(4, "replaceChild() recurse() jsn::value_t is not jsn::object_t;  value=" << boost::apply_visitor(
                    jsn::print_visitor(), childValue));

      throw notfound_exception("JSONDocument") << "Replace failed for" << path_tokens.at(currentDepth)
                                               << ", replaceChild() recurse() jsn::value_t is not jsn::object_t;  "
                                                  "value="
                                               << boost::apply_visitor(jsn::print_visitor(), childValue);
    }

    auto& childDocument = boost::get<jsn::object_t>(childValue);

    tmpJson.clear();
    TRACE_(4, "replaceChild() recurse() args childValue=<" << (jsn::JsonWriter().write(childDocument, tmpJson), tmpJson)
                                                           << ">");

    if (childDocument.count(path_tokens.at(currentDepth)) == 0) {
      TRACE_(4, "replaceChild() recurse() Error: Replace failed for " << path_tokens.at(currentDepth)
                                                                      << ", search path =<" << path << ">.");

      throw notfound_exception("JSONDocument") << "Replace failed for " << path_tokens.at(currentDepth)
                                               << ", search path =<" << path << ">.";
    }

    auto& matchedValue = childDocument.at(path_tokens.at(currentDepth));

    if (currentDepth == 0) {
      matchedValue.swap(newValue);
      return newValue;
    }

    return recurse(matchedValue, currentDepth - 1);
  };

  auto replaced_value = recurse(document, path_tokens.size() - 1);

  TRACE_(4, "replaceChild() found child value=" << boost::apply_visitor(jsn::print_visitor(), replaced_value));

  tmpJson.clear();
  jsn::JsonWriter().write(boost::get<jsn::object_t>(document), tmpJson);
  TRACE_(4, "replaceChild() resultDocument=<" << tmpJson << ">");

  auto return_json = std::string{};
  if (replaced_value.type() == typeid(jsn::object_t)) {
    auto const& replaced_object = boost::get<jsn::object_t>(replaced_value);
    if (!replaced_object.empty())
      jsn::JsonWriter().write(replaced_object, return_json);
    else
      return_json = "{}";
  } else {
    auto replaced_object = jsn::object_t{};
    replaced_object["replaced"] = replaced_value;
    jsn::JsonWriter().write(replaced_object, return_json);
  }

  TRACE_(4, "replaceChild() replacedChild=<" << tmpJson << ">");

  _json_buffer.swap(tmpJson);

  TRACE_(4, "replaceChild() Replace succeeded.");
  return {return_json};
}

// returns inserted child
JSONDocument JSONDocument::insertChild(JSONDocument const& newChild, path_t const& path) {
  assert(!path.empty());
  assert(!newChild._json_buffer.empty());

  TRACE_(2, "insertChild() begin _json_buffer=<" << _json_buffer << ">");
  TRACE_(2, "insertChild() args  newChild=<" << newChild._json_buffer << ">");
  TRACE_(2, "insertChild() args  path=<" << path << ">");

  auto path_tokens = _split_path(path);

  if (path_tokens.empty()) throw cet::exception("JSONDocument") << "Invalid path; path=" << path;

  std::reverse(path_tokens.begin(), path_tokens.end());

  auto document = jsn::value_t{jsn::object_t{}};

  if (!jsn::JsonReader().read(_json_buffer, boost::get<jsn::object_t>(document))) {
    TRACE_(4, "insertChild()"
                  << " Invalid initialDocument");

    throw cet::exception("JSONDocument") << " Invalid initialDocument; json_buffer=" << _json_buffer;
  }

  auto tmpValue = jsn::value_t{jsn::object_t{}};

  if (!jsn::JsonReader().read(newChild._json_buffer, boost::get<jsn::object_t>(tmpValue))) {
    TRACE_(4, "insertChild()"
                  << " Invalid newChild document");

    throw cet::exception("JSONDocument") << " Invalid newChild document; json_buffer=" << newChild._json_buffer;
  }

  auto newValue = jsn::value_t{};

  if (boost::get<jsn::object_t>(tmpValue).count("payload") == 1) {
    newValue = boost::get<jsn::object_t>(tmpValue).at("payload");
    if (newValue.type() == typeid(jsn::object_t) && boost::get<jsn::object_t>(newValue).count(path_tokens.at(0)) == 1)
      newValue = jsn::value_t{boost::get<jsn::object_t>(newValue).at(path_tokens.at(0))};
  } else if (boost::get<jsn::object_t>(tmpValue).size() == 1) {
    newValue = boost::get<jsn::object_t>(tmpValue).begin()->value;
  } else {
    newValue = tmpValue;
    TRACE_(4, "insertChild()"
                  << " Insert payload <" << newChild._json_buffer << ">");
  }

  TRACE_(4, "insertChild() new child value=" << boost::apply_visitor(jsn::print_visitor(), newValue));

  auto tmpJson = std::string{};

  std::function<jsn::value_t(jsn::value_t&, std::size_t)> recurse = [&](jsn::value_t& childValue,
                                                                        std::size_t currentDepth) {
    TRACE_(4, "insertChild() recurse() args currentDepth=" << currentDepth);

    if (currentDepth == 0 && childValue.type() != typeid(jsn::object_t)) {
      TRACE_(4, "insertChild() recurse() jsn::value_t is not jsn::object_t;  value=" << boost::apply_visitor(
                    jsn::print_visitor(), childValue));

      throw notfound_exception("JSONDocument") << "Insert failed for" << path_tokens.at(currentDepth)
                                               << ", insertChild() recurse() jsn::value_t is not jsn::object_t;  "
                                                  "value="
                                               << boost::apply_visitor(jsn::print_visitor(), childValue);
    }

    auto& childDocument = boost::get<jsn::object_t>(childValue);

    tmpJson.clear();
    TRACE_(4, "insertChild() recurse() args childValue=<" << (jsn::JsonWriter().write(childDocument, tmpJson), tmpJson)
                                                          << ">");

    auto numberOfChildren = childDocument.count(path_tokens.at(currentDepth));

    if (currentDepth == 0 && numberOfChildren != 0) {
      TRACE_(4, "insertChild() recurse() Error: Insert failed for" << path_tokens.at(currentDepth) << ", search path =<"
                                                                   << path << ">; Child exists, call replace instead.");

      throw notfound_exception("JSONDocument") << "Insert failed for" << path_tokens.at(currentDepth)
                                               << ", search path =<" << path << ">; Child exists, call replace instead";

    } else if (currentDepth != 0 && numberOfChildren == 0) {
      TRACE_(4, "insertChild() recurse() Error: Insert failed for" << path_tokens.at(currentDepth) << ", search path =<"
                                                                   << path << ">.");

      throw notfound_exception("JSONDocument") << "Insert failed for" << path_tokens.at(currentDepth)
                                               << ", search path =<" << path << ">.";
    } else if (currentDepth == 0 && numberOfChildren == 0) {
      childDocument[path_tokens.at(currentDepth)] = newValue;
      return newValue;
    }

    auto& matchedValue = childDocument.at(path_tokens.at(currentDepth));

    return recurse(matchedValue, currentDepth - 1);
  };

  auto inserted_value = recurse(document, path_tokens.size() - 1);

  TRACE_(4, "insertChild() found child value=" << boost::apply_visitor(jsn::print_visitor(), inserted_value));

  tmpJson.clear();
  jsn::JsonWriter().write(boost::get<jsn::object_t>(document), tmpJson);
  TRACE_(4, "insertChild() resultDocument=<" << tmpJson << ">");

  auto return_json = std::string{};
  if (inserted_value.type() == typeid(jsn::object_t)) {
    auto const& replaced_object = boost::get<jsn::object_t>(inserted_value);
    if (!replaced_object.empty())
      jsn::JsonWriter().write(replaced_object, return_json);
    else
      return_json = "{}";
  } else {
    auto replaced_object = jsn::object_t{};
    replaced_object["replaced"] = inserted_value;
    jsn::JsonWriter().write(replaced_object, return_json);
  }

  TRACE_(4, "insertChild() insertChild=<" << tmpJson << ">");

  _json_buffer.swap(tmpJson);

  TRACE_(4, "insertChild() Insert succeeded.");
  return {return_json};
}

// returns old child
JSONDocument JSONDocument::deleteChild(path_t const& path) {
  assert(!path.empty());

  TRACE_(3, "deleteChild() begin _json_buffer=<" << _json_buffer << ">");
  TRACE_(3, "deleteChild() args  path=<" << path << ">");

  auto path_tokens = _split_path(path);

  if (path_tokens.empty()) throw cet::exception("JSONDocument") << "Invalid path; path=" << path;

  std::reverse(path_tokens.begin(), path_tokens.end());

  auto document = jsn::value_t{jsn::object_t{}};

  if (!jsn::JsonReader().read(_json_buffer, boost::get<jsn::object_t>(document))) {
    TRACE_(3, "deleteChild()"
                  << " Invalid initialDocument");

    throw cet::exception("JSONDocument") << " Invalid initialDocument; json_buffer=" << _json_buffer;
  }

  auto tmpJson = std::string{};

  std::function<jsn::value_t(jsn::value_t&, std::size_t)> recurse = [&](jsn::value_t& childValue,
                                                                        std::size_t currentDepth) {
    TRACE_(3, "deleteChild() recurse() args currentDepth=" << currentDepth);

    if (currentDepth == 0 && childValue.type() != typeid(jsn::object_t)) {
      TRACE_(3, "deleteChild() recurse() jsn::value_t is not jsn::object_t;  value=" << boost::apply_visitor(
                    jsn::print_visitor(), childValue));

      throw notfound_exception("JSONDocument") << "Replace failed for " << path_tokens.at(currentDepth)
                                               << ", deleteChild() recurse() jsn::value_t is not jsn::object_t;  "
                                                  "value="
                                               << boost::apply_visitor(jsn::print_visitor(), childValue);
    }

    auto& childDocument = boost::get<jsn::object_t>(childValue);

    tmpJson.clear();
    TRACE_(3, "deleteChild() recurse() args childValue=<" << (jsn::JsonWriter().write(childDocument, tmpJson), tmpJson)
                                                          << ">");

    if (childDocument.count(path_tokens.at(currentDepth)) == 0) {
      TRACE_(3, "deleteChild() recurse() Error: Replace failed for " << path_tokens.at(currentDepth)
                                                                     << ", search path =<" << path << ">.");

      throw notfound_exception("JSONDocument") << "Replace failed for " << path_tokens.at(currentDepth)
                                               << ", search path =<" << path << ">.";
    }

    auto& matchedValue = childDocument.at(path_tokens.at(currentDepth));

    if (currentDepth == 0) {
      return childDocument.delete_at(path_tokens.at(currentDepth));
    }

    return recurse(matchedValue, currentDepth - 1);
  };

  auto deleted_value = recurse(document, path_tokens.size() - 1);

  TRACE_(3, "deleteChild() deleted child value=" << boost::apply_visitor(jsn::print_visitor(), deleted_value));

  tmpJson.clear();
  jsn::JsonWriter().write(boost::get<jsn::object_t>(document), tmpJson);
  TRACE_(3, "deleteChild() resultDocument=<" << tmpJson << ">");

  auto return_json = std::string{};
  if (deleted_value.type() == typeid(jsn::object_t)) {
    auto const& deleted_object = boost::get<jsn::object_t>(deleted_value);
    if (!deleted_object.empty())
      jsn::JsonWriter().write(deleted_object, return_json);
    else
      return_json = "{}";
  } else {
    auto deleted_object = jsn::object_t{};
    deleted_object["deleted"] = deleted_value;
    jsn::JsonWriter().write(deleted_object, return_json);
  }

  if (path == "_id") std::replace(return_json.begin(), return_json.end(), '$', '_');

  TRACE_(3, "deleteChild() deletedChild=<" << return_json << ">");

  _json_buffer.swap(tmpJson);

  TRACE_(3, "deleteChild() Delete succeeded.");
  return {return_json};
}

// returns added child
JSONDocument JSONDocument::appendChild(JSONDocument const& newChild, path_t const& path) {
  assert(!path.empty());

  TRACE_(13, "appendChild() begin _json_buffer=<" << _json_buffer << ">");
  TRACE_(13, "appendChild() args  child=<" << newChild._json_buffer << ">");
  TRACE_(13, "appendChild() args  path=<" << path << ">");

  auto tmpValue = jsn::value_t{jsn::object_t{}};

  if (!jsn::JsonReader().read(newChild._json_buffer, boost::get<jsn::object_t>(tmpValue))) {
    TRACE_(4, "appendChild()"
                  << " Invalid newChild document");

    throw cet::exception("JSONDocument") << " Invalid newChild document; json_buffer=" << newChild._json_buffer;
  }

  auto newValue = jsn::value_t{};

  if (boost::get<jsn::object_t>(tmpValue).count("payload") == 1) {
    newValue = boost::get<jsn::object_t>(tmpValue).at("payload");
  } else {
    newValue = tmpValue;
    TRACE_(4, "appendChild()"
                  << " Invalid newChild document, found invalid insert payload.");

    throw cet::exception("JSONDocument") << " Invalid newChild document, found invalid insert payload; "
                                            "json_buffer="
                                         << newChild._json_buffer;
  }

  TRACE_(4, "appendChild() new child value=" << boost::apply_visitor(jsn::print_visitor(), newValue));

  auto searchResults = findChild(path);

  auto documentAST = jsn::object_t{};

  if (!jsn::JsonReader().read(searchResults._json_buffer, documentAST)) {
    TRACE_(13, "appendChild()"
                   << " Invalid initialDocument");

    throw cet::exception("JSONDocument") << " Invalid initialDocument; json_buffer=" << searchResults._json_buffer;
  }

  if (documentAST.size() != 1) {
    TRACE_(13, "appendChild() Too many elements in the results view, path=<" << path << ">");
    throw cet::exception("JSONDocument") << "Too many elements in the results view, path=<" << path << ">";
  }

  auto firstResultIt = documentAST.begin();

  if (firstResultIt->value.type() != typeid(jsn::array_t)) {
    TRACE_(13, "appendChild() Not an array element, path=<" << path << ">");
    throw cet::exception("JSONDocument") << "Failed to appendChild(); target path is not an array_t element, "
                                            "path=<"
                                         << path << ">";
  }

  auto updatedDocument = jsn::object_t{};
  updatedDocument["payload"] = boost::get<jsn::array_t>(firstResultIt->value);
  boost::get<jsn::array_t>(updatedDocument.at("payload")).push_back(newValue);

  auto updatedDocument_json = std::string{};
  jsn::JsonWriter().write(updatedDocument, updatedDocument_json);
  TRACE_(3, "appendChild() updatedChild=<" << updatedDocument_json << ">");

  auto return_value = std::string{};

  jsn::JsonWriter().write(boost::get<jsn::object_t>(newValue), return_value);

  TRACE_(3, "appendChild() newChild=<" << return_value << ">");

  auto updatedChild = JSONDocument{updatedDocument_json};

  TRACE_(13, "appendChild() updatedChild=<" << updatedChild._json_buffer << ">");

  replaceChild(updatedChild, path);

  return {return_value};
}

// returns removed child
JSONDocument JSONDocument::removeChild(JSONDocument const& delChild, path_t const& path) {
  assert(!path.empty());

  TRACE_(13, "removeChild() begin _json_buffer=<" << _json_buffer << ">");
  TRACE_(13, "removeChild() args  child=<" << delChild._json_buffer << ">");
  TRACE_(13, "removeChild() args  path=<" << path << ">");

  auto tmpValue = jsn::value_t{jsn::object_t{}};

  if (!jsn::JsonReader().read(delChild._json_buffer, boost::get<jsn::object_t>(tmpValue))) {
    TRACE_(4, "removeChild()"
                  << " Invalid delChild document");

    throw cet::exception("JSONDocument") << " Invalid delChild document; json_buffer=" << delChild._json_buffer;
  }

  auto delValue = jsn::value_t{};

  if (boost::get<jsn::object_t>(tmpValue).count("payload") == 1) {
    delValue = boost::get<jsn::object_t>(tmpValue).at("payload");
  } else {
    delValue = tmpValue;
    TRACE_(4, "removeChild()"
                  << " Invalid delChild document, found invalid delete payload.");

    throw cet::exception("JSONDocument") << " Invalid delChild document, found invalid delete payload; "
                                            "json_buffer="
                                         << delChild._json_buffer;
  }

  TRACE_(4, "removeChild() new delete value=" << boost::apply_visitor(jsn::print_visitor(), delValue));

  auto searchResults = findChild(path);

  auto documentAST = jsn::object_t{};

  if (!jsn::JsonReader().read(searchResults._json_buffer, documentAST)) {
    TRACE_(13, "removeChild()"
                   << " Invalid initialDocument");

    throw cet::exception("JSONDocument") << " Invalid initialDocument; json_buffer=" << searchResults._json_buffer;
  }

  if (documentAST.size() != 1) {
    TRACE_(13, "appendChild() Too many elements in the results view, path=<" << path << ">");
    throw cet::exception("JSONDocument") << "Too many elements in the results view, path=<" << path << ">";
  }

  auto firstResultIt = documentAST.begin();

  if (firstResultIt->value.type() != typeid(jsn::array_t)) {
    TRACE_(13, "appendChild() Not an array element, path=<" << path << ">");
    throw cet::exception("JSONDocument") << "Failed to appendChild(); target path is not an array_t element, "
                                            "path=<"
                                         << path << ">";
  }

  auto updatedDocument = jsn::object_t{};

  auto arrayAST = boost::get<jsn::array_t>(firstResultIt->value);

  updatedDocument["payload"] = jsn::array_t{};

  auto returnAST = jsn::object_t{};
  returnAST["0"] = jsn::array_t{};

  std::for_each(arrayAST.begin(), arrayAST.end(), [&](auto const& element) {
    if (matches(delValue, element)) {
      boost::get<jsn::array_t>(returnAST.at("0")).push_back(element);
    } else {
      boost::get<jsn::array_t>(updatedDocument.at("payload")).push_back(element);
    }
  });

  auto updatedDocument_json = std::string{};
  jsn::JsonWriter().write(updatedDocument, updatedDocument_json);
  TRACE_(3, "removeChild() updatedChild=<" << updatedDocument_json << ">");

  auto return_value = std::string{};

  jsn::JsonWriter().write(returnAST, return_value);

  TRACE_(3, "removeChild() delChild=<" << return_value << ">");

  auto updatedChild = JSONDocument{updatedDocument_json};

  TRACE_(13, "removeChild() updatedChild=<" << updatedChild._json_buffer << ">");

  replaceChild(updatedChild, path);

  return {return_value};
}

bool JSONDocument::equals(JSONDocument const& other) const {
  TRACE_(10, "operator==() begin _json_buffer=<" << _json_buffer << ">");
  TRACE_(10, "operator==() args  other.__json_buffer=<" << other._json_buffer << ">");

  if (other._json_buffer == _json_buffer) return true;

  auto selfAST = jsn::object_t{};
  if (!jsn::JsonReader().read(_json_buffer, selfAST)) {
    TRACE_(10, "matches()"
                   << " Invalid initialDocument");

    throw cet::exception("JSONDocument") << " Invalid initialDocument; json_buffer=" << _json_buffer;
  }

  auto otherAST = jsn::object_t{};

  if (!jsn::JsonReader().read(other._json_buffer, otherAST)) {
    TRACE_(10, "matches()"
                   << " Invalid other initialDocument");

    throw cet::exception("JSONDocument") << " Invalid other initialDocument; json_buffer=" << other._json_buffer;
  }

  auto result = jsn::operator==(selfAST, otherAST);

  TRACE_(10, "matches() JSON buffers are " << (result.first ? "equal." : "not equal."));

  if (result.first) return true;

  TRACE_(10, "matches() Error message=<" << result.second << ">");

  return false;
}

std::string JSONDocument::value_at(JSONDocument const& document, std::size_t index) {
  TRACE_(14, "value_at() begin _json_buffer=<" << document._json_buffer << ">");
  TRACE_(14, "value_at() begin index=<" << index << ">");

  auto json_buffer = document._json_buffer;

  // needed for compatibility with mongodb
  if (json_buffer.find("\"0\"") != std::string::npos) json_buffer.replace(json_buffer.find("\"0\""), 3, "\"payload\"");

  if (json_buffer.find("\"payload\"") == std::string::npos)
    json_buffer = "{ \"payload\":" + document._json_buffer + "}";

  TRACE_(14, "value_at() json_buffer=<" << json_buffer << ">");

  auto tmpObject = jsn::object_t{};

  if (!jsn::JsonReader().read(json_buffer, tmpObject)) {
    TRACE_(14, "value_at()"
                   << " Invalid document");

    throw cet::exception("JSONDocument") << " Invalid document; json_buffer=" << document._json_buffer;
  }

  if (tmpObject.count("payload") != 1) {
    TRACE_(14, "value_at()"
                   << " Invalid document.");

    throw cet::exception("JSONDocument") << " Invalid document; json_buffer=" << document._json_buffer;
  }

  auto const& valueArray = boost::get<jsn::array_t>(tmpObject.at("payload"));

  TRACE_(14, "value_at() new child value=" << boost::apply_visitor(jsn::print_visitor(), tmpObject.at("payload")));

  if (valueArray.empty()) {
    TRACE_(14, "value_at() Failed to call value_at(); valueArray is empty, document=<" << document._json_buffer << ">");
    throw cet::exception("JSONDocument") << "Failed to call value_at(); valueArray is empty, document=<"
                                         << document._json_buffer << ">";
  }

  if (valueArray.size() < index) {
    TRACE_(14, "value_at() Failed to call value_at(); not enough elements, document=<" << document._json_buffer << ">");
    throw cet::exception("JSONDocument") << "Failed to call value_at(); not enough elements, document=<"
                                         << document._json_buffer << ">";
  }

  auto pos = valueArray.begin();
  std::advance(pos, index);

  auto return_value = std::string{};

  if (pos->type() == typeid(jsn::object_t))
    jsn::JsonWriter().write(boost::get<jsn::object_t>(*pos), return_value);
  else
    return_value = boost::apply_visitor(jsn::tostring_visitor(), *pos);

  return return_value;
}

namespace debug {
void enableJSONDocument() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeM", trace_mode::modeM);

  TRACE_(0, "artdaq::database::JSONDocument trace_enable");
}
}

}  // namespace jsonutils
}  // namespace database
}  // namespace artdaq
