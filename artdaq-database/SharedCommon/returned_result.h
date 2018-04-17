#ifndef _ARTDAQ_DATABASE_DATAFORMATS_COMMON_RESULT_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_COMMON_RESULT_H_

#include <sstream>
#include <string>
#include <utility>

namespace artdaq {
namespace database {

namespace result {
constexpr auto msg_Missing = "{\"message\":\"Missing\"}";
constexpr auto msg_Ignored = "{\"message\":\"Ignored\"}";
constexpr auto msg_Updated = "{\"message\":\"Updated\"}";
constexpr auto msg_Added = "{\"message\":\"Added\"}";
constexpr auto msg_Removed = "{\"message\":\"Removed\"}";
constexpr auto msg_Success = "{\"message\":\"Success\"}";
constexpr auto msg_Failure = "{\"message\":\"Failure\"}";
constexpr auto msg_AlreadyExist = "{\"message\":\"AlreadyExist\"}";
}  // namespace result

using result_t = std::pair<bool, std::string>;

result_t Failure(std::ostringstream const&);
result_t Success(std::ostringstream const&);
result_t Failure(std::string const& = result::msg_Failure);
result_t Success(std::string const& = result::msg_Success);
void ThrowOnFailure(result_t const&);
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_DATAFORMATS_COMMON_RESULT_H_ */
