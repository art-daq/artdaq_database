#include "artdaq-database/BasicTypes/data_json.h"
#include "artdaq-database/BasicTypes/data_json_fusion.h"

#include "artdaq-database/BasicTypes/data_fhicl.h"

#include <utility>
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"

#include "artdaq-database/DataFormats/Fhicl/fhicl_common.h"
#include "artdaq-database/DataFormats/Fhicl/fhicljsondb.h"

#include "artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h"

#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/BasicTypes/base64.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "data_fhicl.cpp"

namespace regex {
constexpr auto parse_base64data = R"lit([\s\S]*"base64"\s*:\s*"(\S*?)")lit";
}

namespace artdaq {
namespace database {
namespace basictypes {

template <>
bool JsonData::convert_to(FhiclData& fhicl) const {
  using artdaq::database::fhicljson::json_to_fhicl;

  return json_to_fhicl(json_buffer, fhicl.fhicl_buffer, fhicl.fhicl_file_name);
}

template <>
bool JsonData::convert_from(FhiclData const& fhicl) {
  using artdaq::database::fhicljson::fhicl_to_json;

  return fhicl_to_json(fhicl.fhicl_buffer, fhicl.fhicl_file_name, json_buffer);
}

FhiclData::FhiclData(std::string buffer) : fhicl_buffer{std::move(buffer)} {}

FhiclData::operator std::string const&() const { return fhicl_buffer; }

FhiclData::FhiclData(JsonData const& document) {
  namespace literal = artdaq::database::dataformats::literal;

  confirm(!document.empty());

  TLOG(11) << "FHICL document=" << document;

  auto ex = std::regex(regex::parse_base64data);

  auto results = std::smatch();

  if (!std::regex_search(document.json_buffer, results, ex)) {
    throw ::fhicl::exception(::fhicl::parse_error, literal::document)
        << ("JSON to FHICL convertion error, regex_search()==false; JSON buffer: " + document.json_buffer);
  }

  if (results.size() != 1) {
    throw ::fhicl::exception(::fhicl::parse_error, literal::document)
        << ("JSON to FHICL convertion error, regex_search().size()!=1; JSON buffer: " + document.json_buffer);
  }

  auto base64 = std::string(results[0]);
  TLOG(12) << "FHICL base64=" << base64;

  auto json = base64_decode(base64);
  TLOG(13) << "FHICL  json=" << json;

  JsonData(json).convert_to(*this);

  TLOG(14) << "FHICL fhicl=" << fhicl_buffer;
}

FhiclData::operator JsonData() const {
  namespace literal = artdaq::database::dataformats::literal;

  TLOG(15) << "FHICL fhicl=" << fhicl_buffer;

  auto json = JsonData("");

  if (!json.convert_from(*this)) {
    throw ::fhicl::exception(::fhicl::parse_error, literal::data) << ("FHICL to JSON convertion error; FHICL buffer: " + this->fhicl_buffer);
  }

  TLOG(16) << "FHICL  json=" << json;

  auto collection = std::string("FhiclData_") + type_version();

  auto base64 = base64_encode(fhicl_buffer);
  TLOG(17) << "FHICL base64=" << base64;

  std::ostringstream os;

  os << json;

  TLOG(15) << "FHICL document=" << os.str();

  return {os.str()};
}

std::istream& operator>>(std::istream& is, FhiclData& data) {
  auto str = std::string(std::istreambuf_iterator<char>(is), {});
  auto json = JsonData(str);
  data = FhiclData(json);
  return is;
}

std::ostream& operator<<(std::ostream& os, FhiclData const& data) {
  os << data.fhicl_buffer;
  return os;
}

}  // namespace basictypes
}  // namespace database
}  // namespace artdaq
