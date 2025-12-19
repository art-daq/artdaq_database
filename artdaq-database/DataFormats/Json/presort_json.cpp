#include "artdaq-database/DataFormats/Json/presort_json.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/Json/json_writer.h"

#include <algorithm>
#include <string>
#include <string_view>

namespace artdaq {
namespace database {
namespace json {
namespace {

/// @brief Extract the "name" field value from a JSON object AST
/// @param obj The object_t to extract from
/// @return The name string, or empty string if not found
[[nodiscard]] std::string extract_name_from_ast(object_t const& obj) {
  auto it = obj.find("name");
  if (it == obj.end()) {
    return "";
  }

  // Try to extract string value
  try {
    return boost::get<std::string>(it->value);
  } catch (boost::bad_get const&) {
    return "";
  }
}

/// @brief Sort comparison functor for array elements by "name" field
struct NameComparator {
  bool operator()(value_t const& a, value_t const& b) const {
    std::string name_a, name_b;

    try {
      name_a = extract_name_from_ast(boost::get<object_t>(a));
    } catch (boost::bad_get const&) {
      name_a = "";
    }

    try {
      name_b = extract_name_from_ast(boost::get<object_t>(b));
    } catch (boost::bad_get const&) {
      name_b = "";
    }

    return name_a < name_b;
  }
};

}  // anonymous namespace

std::string presort_gui_json_by_name(std::string_view json) {
  // Parse JSON into AST
  object_t ast;
  if (!JsonReader{}.read(std::string(json), ast)) {
    return std::string(json);  // Return original on parse failure
  }

  // Find the "search" key
  auto search_it = ast.find("search");
  if (search_it == ast.end()) {
    return std::string(json);  // No "search" key found
  }

  // Get the array value
  array_t* search_array = nullptr;
  try {
    search_array = &boost::get<array_t>(search_it->value);
  } catch (boost::bad_get const&) {
    return std::string(json);  // "search" is not an array
  }

  if (search_array->empty()) {
    return std::string(json);  // Empty array, nothing to sort
  }

  // Sort the array by "name" field
  // array_t uses std::list internally, so use list::sort()
  search_array->values.sort(NameComparator{});

  // Serialize back to JSON
  std::string result;
  if (!JsonWriter{}.write(ast, result)) {
    return std::string(json);  // Return original on serialization failure
  }

  return result;
}

}  // namespace json
}  // namespace database
}  // namespace artdaq
