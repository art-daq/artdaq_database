#ifndef _ARTDAQ_DATABASE_DOCRECORD_EXCEPTIONS_H_
#define _ARTDAQ_DATABASE_DOCRECORD_EXCEPTIONS_H_

#include "cetlib/coded_exception.h"
namespace artdaq {
namespace database {
namespace docrecord {
  
class notfound_exception : public cet::exception {
 public:
  explicit notfound_exception(std::string const& category_) : cet::exception(category_) {}
};

class readonly_exception : public cet::exception {
 public:
  explicit readonly_exception(std::string const& category_) : cet::exception(category_) {}
};

}  // namespace docrecord
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_JSONRECORD_EXCEPTIONS_H_ */
