#ifndef _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_INDEX_H_
#define _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_INDEX_H_

#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/storage_providers.h"

#include <boost/filesystem.hpp>

namespace artdaq {
namespace database {
namespace basictypes {
class JsonData;
}

namespace filesystem {
namespace index {

using artdaq::database::basictypes::JsonData;
using artdaq::database::object_id_t;
using artdaq::database::json::object_t;

class SearchIndex final {
 public:
  SearchIndex(boost::filesystem::path const&);
  ~SearchIndex();

  std::vector<object_id_t> findDocumentIDs(JsonData const&);
  std::vector<std::pair<std::string, std::string>> findAllGlobalConfigurations(JsonData const&);
  std::vector<std::pair<std::string, std::string>> findVersionsByGlobalConfigName(JsonData const&);
  std::vector<std::pair<std::string, std::string>> findVersionsByEntityName(JsonData const&);

  std::vector<std::string> findEntities(JsonData const&);

  bool addDocument(JsonData const&, object_id_t const&);
  bool removeDocument(JsonData const&, object_id_t const&);

  // deleted
  SearchIndex() = delete;
  SearchIndex& operator=(SearchIndex const&) = delete;
  SearchIndex& operator=(SearchIndex&&) = delete;
  SearchIndex(SearchIndex&&) = delete;

 private:
  void _addVersion(object_id_t const&, std::string const&);
  void _addConfiguration(object_id_t const&, std::string const&);
  void _addentity(object_id_t const&, std::string const&);

  void _removeVersion(object_id_t const&, std::string const&);
  void _removeConfiguration(object_id_t const&, std::string const&);
  void _removeentity(object_id_t const&, std::string const&);
  std::vector<object_id_t> _matchVersion(std::string const&) const;
  std::vector<object_id_t> _matchConfiguration(std::string const&) const;
  std::vector<object_id_t> _matchentity(std::string const&) const;
  std::vector<object_id_t> _matchObjectId(std::string const&) const;
  std::vector<object_id_t> _matchObjectIds(std::string const&) const;

  void _build_ouid_map(std::map<std::string, std::string>&, std::string const&) const;

  template <typename TYPE>
  void _make_unique_sorted(jsn::array_t&) const;

  std::vector<std::pair<std::string, std::string>> _indexed_filtered_innerjoin_over_ouid(std::string const&,
                                                                                         std::string const&,
                                                                                         std::string const&) const;
  std::vector<std::string> _filtered_attribute_list(std::string const& attribute, std::string const& attribute_with) const;

 private:
  bool _open(boost::filesystem::path const&);
  bool _create(boost::filesystem::path const&);
  bool _close();
  bool _rebuild(boost::filesystem::path const&);

 private:
  object_t _index;
  boost::filesystem::path _path;
  bool _isDirty;
  bool _isOpen;
};

bool shouldAutoRebuildSearchIndex(bool = false);

template <typename TYPE>
void SearchIndex::_make_unique_sorted(jsn::array_t& ouids) const{
  auto tmp = std::vector<std::string>{};

  tmp.reserve(ouids.size());

  std::for_each(ouids.begin(), ouids.end(),
                [&tmp](jsn::value_t const& value) { tmp.emplace_back(boost::get<TYPE>(value)); });

  std::sort(tmp.begin(), tmp.end());

  tmp.erase(std::unique(tmp.begin(), tmp.end()), tmp.end());

  ouids.erase(ouids.begin(), ouids.end());

  std::for_each(tmp.begin(), tmp.end(), [&ouids](TYPE& value) { ouids.push_back(value); });
}

namespace debug {
void enable();
}
}  // namespace index
}  // namespace filesystem
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_INDEX_H_ */
