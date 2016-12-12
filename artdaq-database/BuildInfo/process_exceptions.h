#ifndef _PROCESS_EXCEPTIONS_H_
#define _PROCESS_EXCEPTIONS_H_
#include "cetlib/coded_exception.h"

namespace artdaq {
namespace database {
class exception : public cet::exception {
 public:
  using cet::exception::Category;

  explicit exception(Category const& category) : cet::exception{category} {}

  exception(Category const& category, std::string const& message) : cet::exception{category, message} {}

  template <typename W>
  exception& operator<<(W const& w) {
    this->append(w);
    return *this;
  }
};

class invalid_option_exception : public exception {
 public:
  using exception::Category;

  explicit invalid_option_exception(Category const& category) : exception{category} {}

  invalid_option_exception(Category const& category, std::string const& message) : exception{category, message} {}
};

class runtime_exception : public exception {
 public:
  using exception::Category;

  explicit runtime_exception(Category const& category) : exception{category} {}

  runtime_exception(Category const& category, std::string const& message) : exception{category, message} {}
};
}
}
#endif  // _PROCESS_EXCEPTIONS_H_
