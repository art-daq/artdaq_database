#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_H_

#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/ConfigurationDB/dboperations.h"
#include "artdaq-database/ConfigurationDB/search_filter.h"

typedef std::pair<bool, std::string> result_pair_t;

result_pair_t load_configuration(std::string const& search_filter);
result_pair_t store_configuration(std::string const& search_filter, std::string const& json_document);
result_pair_t find_global_configurations(std::string const& search_filter);
result_pair_t build_global_configuration_search_filter(std::string const& search_filter);
result_pair_t add_configuration_to_global_configuration(std::string const& search_filter);
result_pair_t create_new_global_configuration(std::string const& search_filter);
result_pair_t find_configuration_versions(std::string const& search_filter);
result_pair_t find_configuration_entities(std::string const& search_filter);
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_H_ */
