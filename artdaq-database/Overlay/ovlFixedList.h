#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLFIXEDLIST_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLFIXEDLIST_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

template <typename T, std::uint32_t mask>
class ovlFixedList final : public ovlKeyValue {
  using List_t = array_t::container_type<T>;
  using ElementUPtr_t = std::unique_ptr<T>;

 public:
  ovlFixedList(object_t::key_type const& /*key*/, value_t& /*array*/);

  // defaults
  ovlFixedList(ovlFixedList&&) = default;
  ~ovlFixedList() = default;

  // utils
  void wipe();
  // bool exists(std::string const& name) {}
  result_t add(ElementUPtr_t&);
  result_t remove(ElementUPtr_t&);

  typename List_t::const_iterator begin() const { return _list.begin(); }
  typename List_t::const_iterator end() const { return _list.end(); }

  // overrides
  std::string to_string() const override;

  // ops
  result_t operator==(ovlFixedList const&) const;

 private:
  List_t make_list(array_t& /*array*/);
  bool init(value_t& /*parent*/);

 private:
  bool _initOK;

  List_t _list;
};

template <typename T, std::uint32_t mask>
ovlFixedList<T, mask>::ovlFixedList(object_t::key_type const& key, value_t& entries)
    : ovlKeyValue(key, entries), _list(make_list(ovlKeyValue::array_value())) {}

template <typename T, std::uint32_t mask>
std::string ovlFixedList<T, mask>::to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(key()) << ": [";

  for (auto const& entry : _list) oss << "\n" << entry.to_string() << ",";

  if (!_list.empty()) oss.seekp(-1, oss.cur);

  oss << "\n]}";

  return oss.str();
}

template <typename T, std::uint32_t mask>
void ovlFixedList<T, mask>::wipe() {
  auto& entries = ovlKeyValue::array_value();
  auto empty = array_t{};
  entries.swap(empty);
  // reattach AST
  _list = make_list(entries);
}

template <typename T, std::uint32_t mask>
typename ovlFixedList<T, mask>::List_t ovlFixedList<T, mask>::make_list(array_t& entries) {
  auto returnValue = ovlFixedList<T, mask>::List_t{};

  for (auto& entry : entries) returnValue.push_back({jsonliteral::value, entry});

  return returnValue;
}

template <typename T, std::uint32_t mask>
result_t ovlFixedList<T, mask>::add(std::unique_ptr<T>& newEntry) {
  confirm(newEntry);

  for (auto& entry : _list) {
    if (entry.name() == newEntry->name()) {
      // entry.timestamp().swap(newEntry->timestamp());
      return Success(msg_Ignored);
    }
  }

  auto& entries = ovlKeyValue::array_value();
  // update AST
  entries.push_back(newEntry->value());
  // reattach AST
  _list = make_list(entries);

  return Success(msg_Added);
}

template <typename T, std::uint32_t mask>
result_t ovlFixedList<T, mask>::remove(std::unique_ptr<T>& oldEntry) {
  confirm(oldEntry);

  if (_list.empty()) Success(msg_Ignored);

  auto& entries = ovlKeyValue::array_value();

  auto oldCount = entries.size();

  entries.erase(std::remove_if(entries.begin(), entries.end(),
                               [&oldEntry](value_t& entry) -> bool {
                                 auto candidateEntry = std::make_unique<T>(jsonliteral::value, entry);
                                 return candidateEntry->name() == oldEntry->name();
                               }),
                entries.end());

  if (oldCount == entries.size()) return Failure(msg_Missing);

  confirm(oldCount == entries.size() + 1);

  oldEntry->object_value()[jsonliteral::removed] = timestamp();

  _list = make_list(entries);

  return Success(msg_Removed);
}

template <typename T, std::uint32_t mask>
result_t ovlFixedList<T, mask>::operator==(ovlFixedList const& other) const {
  if ((useCompareMask() & mask) == mask) return Success();

  std::ostringstream oss;
  oss << "\nEntry lists disagree.";
  auto noerror_pos = oss.tellp();

  if (_list.size() != other._list.size())
    oss << "\n  Entry " << key() << " have different size: self,other=" << _list.size() << "," << other._list.size();

  if (oss.tellp() == noerror_pos &&
      std::equal(_list.cbegin(), _list.end(), other._list.cbegin(), [&oss](auto const& first, auto const& second) -> bool {
        auto result = first == second;
        if (result.first) return true;
        oss << result.second;
        return false;
      }))
    return Success();
  else
    oss << "\n  Entry lists are different";

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLFIXEDLIST_H_ */
