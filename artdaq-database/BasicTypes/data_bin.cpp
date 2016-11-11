#include "artdaq-database/BasicTypes/data_json.h"
#include "artdaq-database/BasicTypes/data_json_fusion.h"

#include "artdaq-database/BasicTypes/data_bin.h"
#include "artdaq-database/BasicTypes/data_bin_fusion.h"

#include "artdaq-database/DataFormats/Bin/bin_common.h"
#include "artdaq-database/DataFormats/Bin/binjsondb.h"

#include "artdaq-database/DataFormats/common/shared_literals.h"

#include "artdaq-database/BasicTypes/base64.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "BTYPES:BinData_C"

namespace regex {
constexpr auto parse_base64data = "[\\s\\S]*\"base64\"\\s*:\\s*\"(\\S*?)\"";
}

namespace artdaq {
namespace database {
namespace basictypes {

template <>
bool JsonData::convert_to(BinData& bin) const {
  using artdaq::database::binjson::json_to_bin;

  return json_to_bin(json_buffer, bin.bin_buffer);
}

template <>
bool JsonData::convert_from(BinData const& bin) {
  using artdaq::database::binjson::bin_to_json;

  return bin_to_json(bin.bin_buffer, json_buffer);
}

BinData::BinData(std::vector<unsigned char> const& buffer) : bin_buffer{buffer} {}

BinData::BinData(JsonData const& document) {
  namespace literal = artdaq::database::dataformats::literal;
  
  assert(!document.json_buffer.empty());

  TRACE_(1, "BIN document=" << document.json_buffer);

  auto ex = std::regex({regex::parse_base64data});

  auto results = std::smatch();

  if (!std::regex_search(document.json_buffer, results, ex))
    throw std::runtime_error("JSON to BIN convertion error, regex_search()==false; JSON buffer: " + document.json_buffer);

  if (results.size() != 1)
    throw std::runtime_error("JSON to BIN convertion error, regex_search().size()!=1; JSON buffer: " + document.json_buffer);

  auto base64 = std::vector<unsigned char>();
  std::string value(results[0]);  
  std::copy(value.begin(), value.end(), std::back_inserter(base64));

  TRACE_(2, "BIN base64=" << base64);

  auto json = base64_decode(base64);
  TRACE_(3, "BIN  json=" << json);

  JsonData(json).convert_to(*this);

  TRACE_(4, "BIN bin=" << bin_buffer);
}

BinData::operator JsonData() const {
  namespace literal = artdaq::database::dataformats::literal;
  
  TRACE_(5, "BIN bin=" << bin_buffer);

  auto json = JsonData("");

  if (!json.convert_from(*this))
    throw std::runtime_error("BIN to JSON convertion error; BIN buffer" );

  TRACE_(6, "BIN  json=" << json.json_buffer);

  auto collection = std::string("BinData_") + type_version();

  auto base64 = base64_encode(bin_buffer);
  TRACE_(7, "BIN base64=" << base64);

  std::ostringstream os;

  os << json.json_buffer;

  TRACE_(8, "BIN document=" << os.str());

  return {os.str()};
}

std::istream& operator>>(std::istream& is, BinData& data) {
  auto str = std::vector<unsigned char>(std::istreambuf_iterator<char>(is), {});
  auto json = JsonData( std::string(str.begin(),str.end()));
  data = BinData(json);
  return is;
}

std::ostream& operator<<(std::ostream& os, BinData const& data) {
    os << data.bin_buffer;
    return os;
}

}  // namespace basictypes
}  // namespace database
}  // namespace artdaq
