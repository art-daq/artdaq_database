#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_SEARCH_FILTER_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_SEARCH_FILTER_H_

#include "artdaq-database/ConfigurationDB/common.h"

namespace artdaq {
namespace database {
namespace configuration {

class filters {
 public:
  using filter_type = std::istream;
  filter_type& filter() { return std::get<filter_type&>(filters); };

 private:
  std::tuple<filter_type&> filters;
};

}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_SEARCH_FILTER_H_ */
