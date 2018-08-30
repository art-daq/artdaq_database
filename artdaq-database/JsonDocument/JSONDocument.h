#ifndef _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_H_
#define _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_H_

#include "artdaq-database/JsonDocument/common.h"

namespace artdaq {
namespace database {
namespace basictypes {
struct JsonData;
}
namespace docrecord {

using artdaq::database::basictypes::JsonData;
using artdaq::database::json::object_t;
using artdaq::database::json::value_t;

class JSONDocumentBuilder;
class JSONDocumentMigrator;

class JSONDocument final {
  friend class JSONDocumentBuilder;
  friend class JSONDocumentMigrator;

 public:
  JSONDocument(JsonData const&);
  JSONDocument(std::string const&);
  JSONDocument(value_t);
  JSONDocument();

  // returns inserted child
  JSONDocument insertChild(JSONDocument const&, path_t const&);
  // returns old child
  JSONDocument replaceChild(JSONDocument const&, path_t const&);
  // returns old child
  JSONDocument deleteChild(path_t const&);
  // returns found child
  JSONDocument findChild(path_t const&) const;
  // returns found child value as a document
  JSONDocument findChildDocument(path_t const&) const;

  // returns added child
  JSONDocument appendChild(JSONDocument const&, path_t const&);
  // returns removed child
  JSONDocument removeChild(JSONDocument const&, path_t const&);

  std::string to_string() const;
  bool empty() const;

  operator std::string() const;

  std::string value() { return JSONDocument::value(self()); }

  bool equals(JSONDocument const&) const;

  bool operator==(JSONDocument const& other) const { return equals(other); }
  bool operator!=(JSONDocument const& other) const { return !(*this == other); }

  template <typename T>
  T value_as(path_t const& path) const;

  // utilities
  bool saveToFile(std::string const&);

  static std::string value(JSONDocument const&);
  static std::string value_at(JSONDocument const&, std::size_t);
  static JSONDocument loadFromFile(std::string const&);

  // defaults
  ~JSONDocument() = default;
  JSONDocument(JSONDocument const&) = default;
  JSONDocument& operator=(JSONDocument&&) = default;
  JSONDocument(JSONDocument&&) = default;

  // deleted
  JSONDocument& operator=(JSONDocument const&) = delete;
  // JSONDocument& operator= ( JSONDocument && ) = delete;
  // JSONDocument ( JSONDocument && ) = delete;

 private:
  value_t readJson(std::string const&);
  std::string writeJson() const;

  std::string const& cached_json_buffer() const;
  value_t const& getPayloadValueForKey(object_t::key_type const& key) const;
  value_t& findChildValue(path_t const&);
  value_t const& findChildValue(path_t const&) const;

  JSONDocument& self() { return *this; }
  JSONDocument const& self() const { return *this; }

 private:
  value_t _value;
  mutable std::string _cached_json_buffer;
  mutable bool _isDirty;
};

template <typename T>
T JSONDocument::value_as(path_t const& path) const {
  return boost::lexical_cast<T>(findChild(path).value());
}

namespace debug {
void JSONDocument();
void JSONDocumentUtils();
}  // namespace debug
std::ostream& operator<<(std::ostream&, JSONDocument const&);

std::vector<path_t> split_path(path_t const&);

}  // namespace docrecord
}  // namespace database
}  // namespace artdaq

namespace {
template <>
inline TraceStreamer& TraceStreamer::operator<<(const artdaq::database::docrecord::JSONDocument& r) {
  std::ostringstream s;
  s << r;
  msg_append(s.str().c_str());
  return *this;
}
}  // namespace

#endif /* _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_H_ */
