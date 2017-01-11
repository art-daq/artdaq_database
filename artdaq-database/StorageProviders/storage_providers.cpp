#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/storage_providers.h"

#include <wordexp.h>
#include <algorithm>
#include <fstream>
#include <regex>
#include <stdexcept>

namespace db = artdaq::database;

std::string db::expand_environment_variables(std::string var) {
  wordexp_t p;
  char** w;

  ::wordexp(var.c_str(), &p, 0);

  w = p.we_wordv;

  std::ostringstream oss;

  for (size_t i = 0; i < p.we_wordc; i++) oss << w[i] << "/";

  ::wordfree(&p);

  return oss.str();
}

db::object_id_t db::extract_oid(std::string const& filter) {
  auto ex = std::regex("^\\{[^:]+:\\s+([\\s\\S]+)\\}$");

  auto results = std::smatch();

  if (!std::regex_search(filter, results, ex)) throw std::logic_error(std::string("Regex ouid search failed; JSON buffer:") + filter);

  if (results.size() != 2) {
    // we are interested in a second match
    TRACE_(12, "value()"
                   << "JSON regex_search() result count=" << results.size());
    for (auto const& result : results) {
      TRACE_(12, "value()"
                     << "JSON regex_search() result=" << result);
    }

    throw std::logic_error(std::string("Regex search failed, regex_search().size()!=1; JSON buffer: ") + filter);
  }

  auto match = std::string(results[1]);

  match.erase(match.find_last_not_of(" \n\r\t") + 1);

  auto dequote = [](auto s) {

    if (s[0] == '"' && s[s.length() - 1] == '"')
      return s.substr(1, s.length() - 2);
    else
      return s;
  };

  match = dequote(match);

  TRACE_(12, "value()"
                 << "JSON regex_search() result=" << match);

  return match;
}

std::string operator"" _quoted(const char* text, std::size_t) { return "\"" + std::string(text) + "\""; }

std::string quoted_(std::string const& text) { return "\"" + text + "\""; }

std::string db::make_database_metadata(std::string const& name, std::string const& uri) {
  assert(!name.empty());
  assert(!uri.empty());

  std::time_t now = std::time(nullptr);
  auto timestamp = std::string(std::asctime(std::localtime(&now)));
  timestamp.pop_back();

  // clang-format off
  std::ostringstream oss;
  oss << "document"_quoted
     << ":"
     << "{";
  oss << "name"_quoted
     << ":"
     << quoted_(name)
     << ",";
  oss << "uri"_quoted
     << ":"
     << quoted_(uri)
     << ",";
  oss << "create_time"_quoted
     << ":" << quoted_(timestamp);
  oss << "}";
  // clang-format on

  return oss.str();
}
