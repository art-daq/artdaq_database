#include <utility>
#include <string>

typedef   std::pair<bool, std::string> result_pair_t;

result_pair_t tojson(std::string const& file_name);
result_pair_t tofhicl(std::string const& file_name, std::string const& json);
