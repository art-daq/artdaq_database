#ifndef _ARTDAQ_DATABASE_DOCRECORD_EXCEPTIONS_H_
#define _ARTDAQ_DATABASE_DOCRECORD_EXCEPTIONS_H_

#include "artdaq-database/SharedCommon/shared_exceptions.h"

namespace artdaq {
namespace database {
namespace docrecord {
using artdaq::database::exception;

class notfound_exception : public exception {
 public:
  explicit notfound_exception(std::string const& category_) : exception(category_) {}
};

class readonly_exception : public exception {
 public:
  explicit readonly_exception(std::string const& category_) : exception(category_) {}
};

}  // namespace docrecord
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_JSONRECORD_EXCEPTIONS_H_ */
