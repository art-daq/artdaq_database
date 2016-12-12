#ifndef _ARTDAQ_DATABASE_FUSION_INTROSPECTION_H_
#define _ARTDAQ_DATABASE_FUSION_INTROSPECTION_H_

#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/include/intrinsic.hpp>
#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/optional.hpp>

// See detais at stackoverflow stackoverflow.com
// http://stackoverflow.com/questions/26380420/boost-fusion-sequence-type-and-name-identification-for-structs-and-class
namespace artdaq {
namespace database {
namespace jsonconvert {
namespace visitor {

using boost::optional;
using boost::fusion::result_of::value_at;
using boost::fusion::extension::struct_member_name;
using boost::fusion::result_of::size;
using boost::fusion::traits::is_sequence;
using boost::fusion::at;

using boost::mpl::eval_if;
using boost::mpl::next;
using boost::mpl::identity;
using boost::mpl::int_;

template <typename Flavour, typename UserData>
struct VisitorApplication;

namespace detail {
template <typename V, typename Enable = void>
struct is_vector : std::false_type {};

template <typename UserData>
struct is_vector<std::vector<UserData>, void> : std::true_type {};

namespace iteration {
// Iteration over a sequence
template <typename Visitor, typename S, typename N>
struct members_impl {
  // Type of the current member
  typedef typename value_at<S, N>::type current_t;
  typedef typename next<N>::type next_t;
  typedef struct_member_name<S, N::value> name_t;

  static inline void handle(Visitor& visitor, S& s) {
    auto name = std::string(name_t::call());
    auto& value = at<N>(s);

    visitor.start_member(name);

    VisitorApplication<Visitor, current_t>::handle(visitor, value);
    visitor.finish_member(name);

    members_impl<Visitor, S, next_t>::handle(visitor, s);
  }
};

// End condition of sequence iteration
template <typename Visitor, typename S>
struct members_impl<Visitor, S, typename size<S>::type> {
  static inline void handle(Visitor&, S&) { /*Nothing to do*/
  }
};

// Iterate over struct/sequence. Base template
template <typename Visitor, typename S>
struct Struct : members_impl<Visitor, S, int_<0>> {};

}  // iteration

template <typename Visitor, typename UserData>
struct array_application {
  typedef array_application<Visitor, UserData> type;

  typedef typename UserData::value_type value_type;

  static inline void handle(Visitor& visitor, UserData& array) {
    visitor.start_array();
    for (auto& element : array) VisitorApplication<Visitor, value_type>::handle(visitor, element);
    visitor.finish_array();
  }
};

template <typename Visitor, typename UserData>
struct struct_application {
  typedef struct_application<Visitor, UserData> type;

  static inline void handle(Visitor& visitor, UserData& data) {
    visitor.start_object();
    iteration::Struct<Visitor, UserData>::handle(visitor, data);
    visitor.finish_object();
  }
};

template <typename Visitor, typename UserData, typename Enable = void>
struct value_application {
  typedef value_application<Visitor, UserData> type;

  static inline void handle(Visitor& visitor, UserData& data) { visitor.value(data); }
};

template <typename Visitor, typename UserData>
struct value_application<Visitor, optional<UserData>> {
  typedef value_application<Visitor, optional<UserData>> type;

  static inline void handle(Visitor& visitor, optional<UserData>& data) {
    if (data) VisitorApplication<Visitor, UserData>::handle(visitor, *data);
    //  else
    ;  // perhaps some default action?
  }
};

template <typename Visitor, typename UserData>
struct select_application {
  typedef
      // typename boost::mpl::eval_if<std::is_array<T>,
      // identity<array_application<Visitor, T>>,
      typename boost::mpl::eval_if<
          detail::is_vector<UserData>, identity<array_application<Visitor, UserData>>,
          typename boost::mpl::eval_if<is_sequence<UserData>, identity<struct_application<Visitor, UserData>>,
                                       identity<value_application<Visitor, UserData>>>>::type type;
};

}  // detail

}  // namespace visitor
}  // core
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_FUSION_INTROSPECTION_H_ */
