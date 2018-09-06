#ifndef _ARTDAQ_DATABASE_SHAREDTYPES_H_
#define _ARTDAQ_DATABASE_SHAREDTYPES_H_

#include <stdexcept>
#include <string>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace artdaq {
namespace database {

using decimal = double;
using integer = int64_t;
namespace sharedtypes {

using basic_key_t = std::string;
using optional_comment_t = boost::optional<std::string>;
using optional_annotation_t = boost::optional<std::string>;

template <typename KEY, typename COMMENT>
struct key_of {
  using key_type = KEY;
  using comment_type = COMMENT;

  key_of() = default;
  key_of(key_of&) = default;
  key_of(key_of const&) = default;
  key_of& operator=(key_of const&) = default;

  key_of(std::string const& k) : key(k) {}

  key_of(std::string const& k, typename comment_type::value_type const& c) : key(k), comment(c) {}

  bool operator==(key_of const& other) const { return other.key == key; }

  key_of& operator=(key_type const& k) {
    key = k;
    return *this;
  }

  operator std::string() const { return key + ", comment=" + comment; }

  key_type key;
  comment_type comment;
};

template <typename KEY, typename COMMENT>
using any_key_of = boost::variant<KEY, key_of<KEY, COMMENT>>;

template <typename VALUE, typename ANNOTATION>
struct value_of {
  using value_type = VALUE;
  using annotation_type = ANNOTATION;

  value_of() = default;
  value_of(value_of&) = default;
  value_of(value_of const&) = default;
  value_of& operator=(value_of const&) = default;

  operator value_type() { return value; };

  value_of(value_type const& v) : value(v) {}
  value_of(value_type const& v, typename annotation_type::value_type const& a) : value(v), annotation(a) {}

  value_type value;
  annotation_type annotation;
};

template <typename VALUE, typename ANNOTATION>
using any_value_of = boost::variant<VALUE, value_of<VALUE, ANNOTATION>>;

template <typename TYPE>
struct vector_of {
  using value_type = TYPE;

  template <typename... Ts>
  using container_type = std::list<Ts...>;

  using collection_type = container_type<value_type>;

  using const_iterator = typename collection_type::const_iterator;
  using iterator = typename collection_type::iterator;
  using size_type = typename collection_type::size_type;

  bool empty() const { return values.empty(); }

  void swap(vector_of<value_type>& other) { values.swap(other.values); }

  iterator begin() { return values.begin(); }
  iterator end() { return values.end(); }

  value_type& back() { return values.back(); }

  const_iterator begin() const { return values.begin(); }
  const_iterator end() const { return values.end(); }

  value_type const& back() const { return values.back(); }

  iterator erase(iterator position) { return values.erase(position); }
  iterator erase(iterator first, iterator last) { return values.erase(first, last); }

  iterator insert(iterator position, value_type const& val) { return values.insert(position, val); }
  /*
      void reserve ( size_type n ) {
          values.reserve ( n );
      }
  */
  size_type size() const { return values.size(); }

  void push_back(value_type const& val) { return values.push_back(val); }

  collection_type& operator()() { return values; }

  collection_type values;
};

template <typename KEYTYPE, typename VALUETYPE>
struct kv_pair_of {
  using key_type = KEYTYPE;
  using value_type = VALUETYPE;

  static kv_pair_of make(key_type const& key, value_type const& value) { return {key, value}; }

  key_type key;
  value_type value;
};

template <typename KVP>
struct table_of : vector_of<KVP> {
  using value_type = KVP;
  using key_type = typename value_type::key_type;
  using mapped_type = typename value_type::value_type;
  using size_type = typename vector_of<KVP>::size_type;
  using const_iterator = typename vector_of<KVP>::const_iterator;
  using iterator = typename vector_of<KVP>::iterator;

  mapped_type& operator[](key_type const& key) {
    auto& pairs = vector_of<value_type>::values;

    for (auto& pair : pairs) {
      if (key == pair.key) {
        return pair.value;
      }
    }

    pairs.push_back(value_type::make(key, {false}));

    return pairs.back().value;
  }

  mapped_type& at(key_type const& key) {
    auto& pairs = vector_of<value_type>::values;

    for (auto& pair : pairs) {
      if (key == pair.key) {
        return pair.value;
      }
    }

    throw std::out_of_range("Key not found; key=" + key);
  }

  size_type count(key_type const& key) const {
    auto retValue = size_type{0};

    auto& pairs = vector_of<value_type>::values;

    for (auto& pair : pairs) {
      if (key == pair.key) ++retValue;
    }

    return retValue;
  }

  mapped_type const& at(key_type const& key) const {
    auto const& pairs = vector_of<value_type>::values;

    for (auto const& pair : pairs) {
      if (key == pair.key) {
        return pair.value;
      }
    }

    throw std::out_of_range("Key not found; key=" + key);
  }

  iterator find(key_type const& key) {
    auto& pairs = vector_of<value_type>::values;

    return std::find_if(pairs.begin(), pairs.end(), [&key](auto const& pair) { return key == pair.key; });
  }

  const_iterator find(key_type const& key) const {
    auto const& pairs = vector_of<value_type>::values;
    return std::find_if(pairs.begin(), pairs.end(), [&key](auto const& pair) { return key == pair.key; });
  }

  mapped_type delete_at(key_type const& key) {
    auto& pairs = vector_of<value_type>::values;

    auto pair = find(key);

    if (pair == pairs.end()) throw std::out_of_range("Key not found; key=" + key);

    auto deleted = pair->value;

    pairs.erase(pair);

    return deleted;
  }
};

template <typename TABLE_OF, typename VECTOR_OF>
using variant_value_of = boost::variant<boost::recursive_wrapper<TABLE_OF>, boost::recursive_wrapper<VECTOR_OF>, std::string, decimal, integer, bool>;

template <typename A>
struct unwrapper {
  unwrapper(A& a) : any(a) {}

  template <typename T>
  T& value_as();

  template <typename T>
  T& value_as(std::string const& /*child*/);

  template <typename O>
  auto& value(std::string const& key) {
    return boost::get<O>(any).at(key);
  }

  template <typename O, typename T>
  auto& value(std::string const& key) try {
    return boost::get<O>(any).at(key);
  } catch (std::out_of_range&) {
    confirm(!key.empty());
    return boost::get<O>(any)[key] = T{};
  }

  A& any;
};

template <typename A>
unwrapper<A> unwrap(A& any) {
  return unwrapper<A>(any);
}

}  // namespace sharedtypes
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_SHAREDTYPES_H_ */
