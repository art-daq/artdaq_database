#ifndef _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_H_
#define _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_H_

#include "artdaq-database/JsonDocument/common.h"

namespace artdaq {
namespace database {
namespace jsonutils {

bool isValidJson(std::string const&);
std::string filterJson(std::string const&);

using path_t = std::string;

class notfound_exception : public cet::exception {
 public:
  explicit notfound_exception(std::string const& category_) : cet::exception(category_) {}
};

class readonly_exception : public cet::exception {
 public:
  explicit readonly_exception(std::string const& category_) : cet::exception(category_) {}
};

class JSONDocument final {
 public:
  JSONDocument(std::string const& json) : _json_buffer{validate(json)} {}

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

  std::string const& to_string() const { return _json_buffer; };
  std::string value() { return JSONDocument::value(self()); };
  bool isReadonly() const;

  bool equals(JSONDocument const&) const;

  bool operator==(JSONDocument const& other) const { return equals(other); }
  bool operator!=(JSONDocument const& other) const { return !(*this == other); }

  template <typename T>
  T value_as(path_t const& path) const {
    return boost::lexical_cast<T>(findChild(path).value());
  }

  static std::string validate(std::string const&);
  static std::string value(JSONDocument const&);
  static std::string value_at(JSONDocument const&, std::size_t);

  static JSONDocument loadFromFile(std::string const&);

  // defaults
  ~JSONDocument() = default;
  JSONDocument(JSONDocument const&) = default;
  JSONDocument& operator=(JSONDocument&&) = default;
  JSONDocument(JSONDocument&&) = default;

  // deleted
  JSONDocument() = delete;
  JSONDocument& operator=(JSONDocument const&) = delete;
  // JSONDocument& operator= ( JSONDocument && ) = delete;
  // JSONDocument ( JSONDocument && ) = delete;

 private:
  std::vector<path_t> _split_path(path_t const&) const;
  JSONDocument& self() { return *this; }
  JSONDocument const& self() const { return *this; }

 private:
  std::string _json_buffer;
};

namespace debug {
void enableJSONDocument();
}
std::ostream& operator<<(std::ostream&, JSONDocument const&);

}  // namespace jsonutils
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_H_ */
