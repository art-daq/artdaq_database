#ifndef _ARTDAQ_DATABASE_EXCEPTIONS_H_
#define _ARTDAQ_DATABASE_EXCEPTIONS_H_

#include <exception>
#include "cetlib/coded_exception.h"

namespace debug {
std::string current_exception_diagnostic_information();
}

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

class invalid_argument : public exception {
 public:
  explicit invalid_argument(std::string const& category_) : exception(category_) {}
};

class runtime_error : public exception {
 public:
  explicit runtime_error(std::string const& category_) : exception(category_) {}
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
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_EXCEPTIONS_H_ */
