#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSNU:Document_C"

namespace regex {
constexpr auto parse_value = "^\\{[^:]+:\\s+([\\s\\S]+)\\}$";
}

namespace artdaq {
namespace database {
namespace jsonutils {

std::string JSONDocument::validate(std::string const& json) { return filterJson(json); }

std::string JSONDocument::value(JSONDocument const& document) {
  auto ex = std::regex({regex::parse_value});

  auto results = std::smatch();

  if (!std::regex_search(document._json_buffer, results, ex))
    throw cet::exception("JSONDocument") << ("Regex search failed; JSON buffer:" + document._json_buffer);

  if (results.size() != 2) {
    // we are interested in a second match
    TRACE_(12, "value()"
                   << "JSON regex_search() result count=" << results.size());
    for (auto const& result : results) {
      TRACE_(12, "value()"
                     << "JSON regex_search() result=" << result);
    }

    throw cet::exception("JSONDocument") << ("Regex search failed, regex_search().size()!=1; JSON buffer: " +
                                             document._json_buffer);
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

JSONDocument JSONDocument::loadFromFile(std::string const& fileName) {
  assert(!fileName.empty());

  std::ifstream is;
  is.open(fileName.c_str());

  if (!is.good()) {
    is.close();
    TRACE_(11, "loadFromFile()"
                   << "Failed opening a JSON file=" << fileName);

    throw cet::exception("JSONDocument") << "Failed opening a JSON file=" << fileName;
  }

  std::string json_buffer((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  is.close();

  return {json_buffer};
}

std::vector<std::string> JSONDocument::_split_path(std::string const& path) const {
  auto tmp = std::string{path};

  TRACE_(1, "split_path() args path=<" << path << ">");

  std::replace(tmp.begin(), tmp.end(), '.', ' ');

  std::istringstream iss(tmp);

  std::vector<std::string> tokens{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

  if (!tokens.empty()) {
    std::stringstream ss;
    for (auto const& token : tokens) ss << "\"" << token << "\",";
    TRACE_(1, "split_path() loop token=<" << ss.str() << ">");
  } else {
    TRACE_(1, "split_path() token=<empty> ");
  }

  return tokens;
}

bool JSONDocument::isReadonly() const { return false; }


std::ostream& operator<<(std::ostream& os, JSONDocument const& document) {
  os << document.to_string();

  return os;
}

}  // namespace jsonutils
}  // namespace database
}  // namespace artdaq
