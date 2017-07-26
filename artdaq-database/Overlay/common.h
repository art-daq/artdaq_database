#ifndef _ARTDAQ_DATABASE_OVERLAY_COMMON_H_
#define _ARTDAQ_DATABASE_OVERLAY_COMMON_H_

#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/Json/json_types_impl.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/sharedcommon_common.h"

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::json::array_t;
using artdaq::database::json::type_t;

namespace artdaq {
namespace database {
namespace overlay {

namespace jsonliteral = artdaq::database::dataformats::literal;

constexpr auto msg_InvalidArgument = "{\"message\":\"Invalid Argument\"}";
constexpr auto msg_IsReadonly = "{\"message\":\"Document is readonly\"}";
constexpr auto msg_ConvertionError = "{\"message\":\"Conversion error\"}";

using result_t = artdaq::database::result_t;

enum DOCUMENT_COMPARE_FLAGS {
  DOCUMENT_COMPARE_MUTE_TIMESTAMPS = (1 << 0),
  DOCUMENT_COMPARE_MUTE_OUIDS = (1 << 1),
  DOCUMENT_COMPARE_MUTE_COMMENTS = (1 << 2),
  DOCUMENT_COMPARE_MUTE_BOOKKEEPING = (1 << 3),
  DOCUMENT_COMPARE_MUTE_ORIGIN = (1 << 4),
  DOCUMENT_COMPARE_MUTE_VERSION = (1 << 5),
  DOCUMENT_COMPARE_MUTE_CHANGELOG = (1 << 6),
  DOCUMENT_COMPARE_MUTE_UPDATES = (1 << 7),
  DOCUMENT_COMPARE_MUTE_CONFIGENTITY = (1 << 8),
  DOCUMENT_COMPARE_MUTE_CONFIGURATION = (1 << 9),
  DOCUMENT_COMPARE_MUTE_ALIAS = (1 << 10),
  DOCUMENT_COMPARE_MUTE_ALIAS_HISTORY = (1 << 11),
  DOCUMENT_COMPARE_MUTE_RUN = (1 << 12),
  DOCUMENT_COMPARE_MUTE_ATTACHMENT = (1 << 13),
  DOCUMENT_COMPARE_MUTE_SEARCH = (1 << 14),
  DOCUMENT_COMPARE_MUTE_RAWDATA = (1 << 15),
  DOCUMENT_COMPARE_MUTE_UPDATE_VALUES = (1 << 16),

  // use with caution
  DOCUMENT_COMPARE_MUTE_COLLECTION = (1 << 28),
  DOCUMENT_COMPARE_MUTE_DATA = (1 << 29),
  DOCUMENT_COMPARE_MUTE_METADATA = (1 << 30)
};

std::uint32_t useCompareMask(std::uint32_t = 0);

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_COMMON_H_ */
