#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLMOVABLELIST_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLMOVABLELIST_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

template <typename T, int mask>
class ovlMovableList final : public ovlKeyValue {
  using List_t = array_t::container_type<T>;
  using ElementUPtr_t = std::unique_ptr<T>;

 public:
  ovlMovableList(object_t::key_type const& /*key*/, value_t& /*object*/);

  // defaults
  ovlMovableList(ovlMovableList&&) = default;
  ~ovlMovableList() = default;

  // utils
  void wipe();
  // bool exists(std::string const& name) {}
  result_t add(ElementUPtr_t&);
  result_t remove(ElementUPtr_t&);

  // overrides
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlMovableList const&) const;

  typename List_t::const_iterator begin() const {return _active.begin();}
  typename List_t::const_iterator end() const {return _active.end();}
  
 private:
  List_t make_list(array_t& /*list*/);
  bool init(value_t& /*parent*/);

 private:
  bool _initOK;

  List_t _active;
  List_t _history;
};

template <typename T, int mask>
ovlMovableList<T, mask>::ovlMovableList(object_t::key_type const& key, value_t& object)
    : ovlKeyValue(key, object),
      _initOK(init(object)),
      _active(make_list(ovlKeyValue::value_as<array_t>(jsonliteral::active))),
      _history(make_list(ovlKeyValue::value_as<array_t>(jsonliteral::history))) {}

template <typename T, int mask>
bool ovlMovableList<T, mask>::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::active) == 0) obj[jsonliteral::active] = array_t{};

  if (obj.count(jsonliteral::history) == 0) obj[jsonliteral::history] = array_t{};

  return true;
} catch (...) {
  confirm(false);
  throw;
}

template <typename T, int mask>
void ovlMovableList<T, mask>::wipe() {
  _active = ovlMovableList::List_t{};
  _history = ovlMovableList::List_t{};
}

template <typename T, int mask>
result_t ovlMovableList<T, mask>::add(ElementUPtr_t& newEntry) {
  confirm(newEntry);

  for (auto& entry : _active) {
    if (entry.name() == newEntry->name()) {
      // entry.timestamp().swap(newEntry->timestamp());
      return Success(msg_Ignored);
    }
  }

  auto& entries = ovlKeyValue::value_as<array_t>(jsonliteral::active);
  // update AST
  entries.push_back(newEntry->value());
  // reattach AST
  _active = make_list(entries);

  return Success(msg_Added);
}

template <typename T, int mask>
result_t ovlMovableList<T, mask>::remove(ElementUPtr_t& oldEntry) {
  confirm(oldEntry);

  if (_active.empty()) Success(msg_Ignored);

  auto& entries = ovlKeyValue::value_as<array_t>(jsonliteral::active);

  auto oldCount = entries.size();

  entries.erase(std::remove_if(entries.begin(), entries.end(),
                               [&oldEntry](value_t& entry) -> bool {
                                 auto candidate = std::make_unique<T>(oldEntry->key(), entry);
                                 return candidate->name() == oldEntry->name();
                               }),
                entries.end());

  if (oldCount == entries.size()) return Failure(msg_Missing);

  confirm(oldCount - 1 == entries.size());

  _active = make_list(entries);

  auto& history = ovlKeyValue::value_as<array_t>(jsonliteral::history);

  oldCount = history.size();

  oldEntry->object_value()[jsonliteral::removed] = timestamp();

  history.push_back(oldEntry->value());

  confirm(oldCount + 1 == history.size());

  _history = make_list(history);

  return Success(msg_Removed);
}

template <typename T, int mask>
std::string ovlMovableList<T, mask>::to_string() const noexcept {
  std::ostringstream oss;
  oss << "{" << quoted_(key()) << ": {";

  oss << quoted_(jsonliteral::active) << ": [";
  for (auto const& entry : _active) oss << "\n" << entry.to_string() << ",";
  if (!_active.empty()) oss.seekp(-1, oss.cur);
  oss << "\n],";

  oss << quoted_(jsonliteral::history) << ": [";
  for (auto const& entry : _history) oss << "\n" << entry.to_string() << ",";
  if (!_history.empty()) oss.seekp(-1, oss.cur);
  oss << "\n]\n}\n}";

  return oss.str();
}

template <typename T, int mask>
typename ovlMovableList<T, mask>::List_t ovlMovableList<T, mask>::make_list(array_t& entries) {
  auto returnValue = ovlMovableList::List_t{};

  for (auto& entry : entries) returnValue.push_back({jsonliteral::value, entry});

  return returnValue;
}

template <typename T, int mask>
result_t ovlMovableList<T, mask>::operator==(ovlMovableList const& other) const {
  if ((useCompareMask() & mask) == mask) return Success();

  std::ostringstream oss;
  oss << "\n " << key() << "nodes disagree.";
  auto noerror_pos = oss.tellp();

  if (_active.size() != other._active.size())
    oss << "\n  Different active " << key() << " count: self,other=" << _active.size() << "," << other._active.size();

  auto key_name = key();

  if (oss.tellp() == noerror_pos && std::equal(_active.cbegin(), _active.end(), other._active.cbegin(),
                                               [&oss, &key_name](auto const& first, auto const& second) -> bool {
                                                 auto result = first == second;
                                                 if (result.first) return true;
                                                 oss << "\n  " << key_name
                                                     << " different: self,other=" << first.to_string() << ","
                                                     << second.to_string();
                                                 return false;
                                               }))
    return Success();

  oss << "\n  Debug info:";
  oss << "\n  Self  value:\n" << to_string();
  oss << "\n  Other value:\n" << other.to_string();

  return Failure(oss);
}

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLFIXEDLIST_H_ */