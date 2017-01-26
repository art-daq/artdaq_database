#ifndef _ARTDAQ_DATABASE_JSONCONVERT_SERIALIZER_H_
#define _ARTDAQ_DATABASE_JSONCONVERT_SERIALIZER_H_

#include "artdaq-database/JsonConvert/common.h"
#include "artdaq-database/SharedCommon/helper_functions.h"

namespace artdaq {
namespace database {
namespace jsonconvert {

inline std::string quote(std::string const& s) { return "\"" + s + "\""; }

struct JsonSerializer {
  typedef std::string result_type;

  explicit JsonSerializer(std::ostream& os_) : os(os_) { os << std::boolalpha; }

  void start_member(std::string const& name[[gnu::unused]]) {
    confirm(!name.empty());
    if (write_comma) os << ", ";
    os << quote(name) << ':';
    write_comma = false;
  }

  void finish_member(std::string const& name[[gnu::unused]]) {
    confirm(!name.empty());
    write_comma = true;
  }

  template <typename Value>
  void value(Value const& value) {
    if (write_comma) os << ", ";
    os << value;
    write_comma = true;
  }

  void value(std::string const& value) {
    if (write_comma) os << ", ";
    os << quote(value);
    write_comma = true;
  }

  void value(char const& value) {
    if (write_comma) os << ", ";
    os << quote({value});
    write_comma = true;
  }

  void value(char* const& value) {
    if (write_comma) os << ", ";
    os << quote(value);
    write_comma = true;
  }

  void start_object() {
    if (write_comma) os << ", ";
    os << "{ ";
    ++level;
    write_comma = false;
  }

  void finish_object() {
    os << " }";
    --level;
    write_comma = true;
  }

  void start_array() {
    os << "[ ";
    write_comma = false;
  }

  void finish_array() { os << " ]"; }

 private:
  std::ostream& os;
  int level = 0;
  bool write_comma = false;
};

}  // namespace jsonconvert
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_JSONCONVERT_SERIALIZER_H_ */
