#include "artdaq-database/Overlay/ovlDocument.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace jsonliteral = artdaq::database::dataformats::literal;
namespace ovl = artdaq::database::overlay;
using namespace artdaq::database;
using namespace artdaq::database::overlay;
using namespace artdaq::database::result;
using result_t = artdaq::database::result_t;
using artdaq::database::sharedtypes::unwrap;

ovlDocument::ovlDocument(object_t::key_type const& key, value_t& document)
    : ovlKeyValue(key, document),
      _data{overlay<ovlData>(document, jsonliteral::data)},
      _metadata{overlay<ovlMetadata>(document, jsonliteral::metadata)},
      _search{overlay<ovlSearches, array_t>(document, jsonliteral::search)} {}

ovlData& ovlDocument::data() { return *_data; }

ovlData const& ovlDocument::data() const { return *_data; }

void ovlDocument::swap(ovlDataUPtr_t& data) {
  confirm(data);

  std::swap(_data, data);
}

ovlMetadata& ovlDocument::metadata() { return *_metadata; }

ovlMetadata const& ovlDocument::metadata() const { return *_metadata; }

void ovlDocument::swap(ovlMetadataUPtr_t& metadata) {
  confirm(metadata);

  std::swap(_metadata, metadata);
}

void ovlDocument::make_empty() {
  value_t tmp1 = object_t{};
  auto data = overlay<ovlData>(tmp1, jsonliteral::data);
  std::swap(_data, data);

  value_t tmp2 = object_t{};
  auto metadata = overlay<ovlMetadata>(tmp2, jsonliteral::metadata);
  std::swap(_metadata, metadata);

  value_t tmp3 = array_t{};
  auto search = overlay<ovlSearches, array_t>(tmp3, jsonliteral::metadata);
  std::swap(_search, search);
}

std::string ovlDocument::to_string() const noexcept {
  std::ostringstream oss;
  oss << quoted_(jsonliteral::document) << ":{\n";
  oss << debrace(_data->to_string()) << ",\n";
  oss << debrace(_metadata->to_string()) << ",\n";
  oss << debrace(_search->to_string()) << "\n";
  oss << "}";

  return oss.str();
}

result_t ovlDocument::operator==(ovlDocument const& other) const {
  std::ostringstream oss;
  oss << "\nUser data disagree.";
  auto noerror_pos = oss.tellp();

  auto result = *_data == *other._data;

  if (!result.first) oss << "\n  Data are different:\n  " << result.second;

  result = *_metadata == *other._metadata;

  if (!result.first) oss << "\n  Metadata are different:\n  " << result.second;

  if (oss.tellp() == noerror_pos) return Success();

  //  oss << "\n  Debug info:";
  //  oss << "\n  Self  value: " << to_string();
  //  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}
