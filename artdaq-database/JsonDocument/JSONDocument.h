#ifndef _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_H_
#define _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_H_

#include "artdaq-database/JsonDocument/common.h"

namespace artdaq {
namespace database {
namespace docrecord {

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;

class JSONDocumentBuilder;

class JSONDocument final {
  friend class JSONDocumentBuilder;

 public:
  JSONDocument(std::string const& json) : _value{readJson(json)}, _cached_json_buffer(json) {}
  JSONDocument(value_t const& value) : _value{value}, _cached_json_buffer(writeJson()) {}
  JSONDocument() : _value{object_t{}}, _cached_json_buffer(writeJson()) {}

  // returns inserted child
  JSONDocument insertChild(JSONDocument const&, path_t const&);
  // returns old child
  JSONDocument replaceChild(JSONDocument const&, path_t const&);
  // returns old child
  JSONDocument deleteChild(path_t const&);
  // returns found child
  JSONDocument findChild(path_t const&) const;

  // returns added child
  JSONDocument appendChild(JSONDocument const&, path_t const&);
  // returns removed child
  JSONDocument removeChild(JSONDocument const&, path_t const&);

  std::string to_string() const;
  std::string value() { return JSONDocument::value(self()); };
  bool isReadonly() const;

  bool equals(JSONDocument const&) const;

  bool operator==(JSONDocument const& other) const { return equals(other); }
  bool operator!=(JSONDocument const& other) const { return !(*this == other); }

  template <typename T>
  T value_as(path_t const& path) const;

  // utilities
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
  void update_json_buffer();

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
  std::string _cached_json_buffer;
};

template <typename T>
T JSONDocument::value_as(path_t const& path) const {
  return boost::lexical_cast<T>(findChild(path).value());
}

namespace debug {
void enableJSONDocument();
void enableJSONDocumentUtils();
}
std::ostream& operator<<(std::ostream&, JSONDocument const&);

/*
bool isValidJson(std::string const&);
std::string filterJson(std::string const&);
*/

std::vector<path_t> split_path(path_t const&);

}  // namespace docrecord
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_H_ */
