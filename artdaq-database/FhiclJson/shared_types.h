#ifndef _ARTDAQ_DATABASE_SHAREDTYPES_H_
#define _ARTDAQ_DATABASE_SHAREDTYPES_H_

#include "artdaq-database/FhiclJson/common.h"

#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include <sstream>

namespace artdaq{
namespace database{
namespace sharedtypes{

using basic_key_t = std::string;
using optional_comment_t  = boost::optional<std::string>;
using optional_annotation_t  = boost::optional<std::string>;

template <typename KEY, typename COMMENT>
struct  key_of {
    using key_type = KEY;
    using comment_type = COMMENT;

    key_of() = default;
    key_of(key_of&) = default;
    key_of(key_of const&) = default;
    key_of& operator=(key_of const&) = default;

    key_of(std::string const& k)
        : key(k) {}

    key_of(std::string const& k, typename comment_type::value_type const& c)
        : key(k), comment(c) {}

    bool operator==(key_of const& other) const {
        return other.key == key;
    }

    key_of& operator=(key_type const& k) {
        key = k; return *this;
    }

    key_type      key;
    comment_type  comment;
};

template <typename KEY, typename COMMENT>
using any_key_of = boost::variant <KEY, key_of <KEY, COMMENT>>;

template <typename VALUE, typename ANNOTATION>
struct  value_of {
    using  value_type = VALUE;
    using  annotation_type = ANNOTATION;

    value_of() = default;
    value_of(value_of&) = default;
    value_of(value_of const&) = default;
    value_of& operator=(value_of const&) = default;

    operator value_type() {
        return value;
    };

    value_of(value_type const& v): value(v) {}
    value_of(value_type const& v, typename annotation_type::value_type const& a)
        : value(v), annotation(a) {}
    value_type value;
    annotation_type annotation;
};

template <typename VALUE, typename ANNOTATION>
using any_value_of = boost::variant <VALUE, value_of<VALUE, ANNOTATION>>;

template <typename TYPE>
struct vector_of {
    using value_type = TYPE;
    using values_t = std::vector<value_type>;

    using const_iterator = typename values_t::const_iterator;
    using iterator = typename values_t::iterator;
    using size_type = typename values_t::size_type;

    bool empty() const {
        return values.empty();
    }

    void swap(vector_of<value_type>& other) {
        values.swap(other.values);
    }

    iterator begin() {
        return values.begin();
    }
    iterator end() {
        return values.end();
    }

    const_iterator begin() const {
        return values.begin();
    }
    const_iterator end() const {
        return values.end();
    }

    iterator insert(iterator position,  value_type const& val) {
        return values.insert(position, val);
    }

    void reserve(size_type n) {
        values.reserve(n);
    }

    void push_back(value_type const& val) {
        return values.push_back(val);
    }

    values_t& operator()() {
        return values;
    }

    values_t values;
};

template <typename KEYTYPE, typename VALUETYPE>
struct atom_of {
    using key_type = KEYTYPE;
    using value_type = VALUETYPE;

    static atom_of make_atom(key_type const& key,
                             value_type const& value) {
        return  {key, value};
    }

    key_type   key;
    value_type value;
};

template <typename ATOM>
struct table_of : vector_of<ATOM> {
    using atom_type = ATOM;

    typename atom_type::value_type& operator[](typename atom_type::key_type const& key) {
        auto& atoms = vector_of<ATOM>::values;

        for (auto & atom : atoms) {
            if (key == atom.key)
                return atom.value;
        }

        atoms.push_back(atom_type::make_atom(key, {false}));

        return atoms.back().value;
    }
};

template <typename TABLE, typename SEQUENCE>
using variant_value_of = boost::variant <
                         boost::recursive_wrapper<TABLE>,
                         boost::recursive_wrapper<SEQUENCE>,
                         std::string,
                         double,
                         int,
                         bool
                         >;

} //namespace sharedtypes
} //namespace database
} //namespace artdaq

#endif /* _ARTDAQ_DATABASE_SHAREDTYPES_H_ */

