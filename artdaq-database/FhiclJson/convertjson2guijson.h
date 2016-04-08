#ifndef _ARTDAQ_DATABASE_FHICLJSON_CONVERTJSON2JSONGUI_H_
#define _ARTDAQ_DATABASE_FHICLJSON_CONVERTJSON2JSONGUI_H_

#include "artdaq-database/FhiclJson/common.h"

//#include "artdaq-database/FhiclJson/shared_types.h"
#include "artdaq-database/FhiclJson/json_types.h"
#include "artdaq-database/FhiclJson/shared_literals.h"

namespace artdaq {
namespace database {
namespace fhicljson {

using namespace artdaq::database::json;

struct value_crt {
  value_crt(value_t const& val) : value{val} {}
  value_t const& value;
};

struct value_rt {
  value_rt(value_t& val) : value{val} {}
  value_t& value;
};

using json_any_cref_t = boost::variant<value_t const&,    // VALUE =1
                                       value_crt const&,  // VALUE =1
                                       data_t const&,     // DATA  =2
                                       object_t const&,   // OBJECT=3
                                       array_t const&     // ARRAY =4
                                       >;

using json_any_ref_t = boost::variant<value_t&,   // VALUE =1
                                      value_rt&,  // VALUE =1
                                      data_t&,    // DATA  =2
                                      object_t&,  // OBJECT=3
                                      array_t&    // ARRAY =4
                                      >;

class json_node_t final {
  struct type_visitor : public boost::static_visitor<type_t> {
    type_t operator()(value_t const&) const { return type_t::VALUE; }
    type_t operator()(value_rt const&) const { return type_t::VALUE; }
    type_t operator()(data_t const&) const { return type_t::DATA; }
    type_t operator()(object_t const&) const { return type_t::OBJECT; }
    type_t operator()(array_t const&) const { return type_t::ARRAY; }
    type_t operator()(value_t&) const { return type_t::VALUE; }
    type_t operator()(value_crt const&) const { return type_t::VALUE; }
    type_t operator()(data_t&) const { return type_t::DATA; }
    type_t operator()(object_t&) const { return type_t::OBJECT; }
    type_t operator()(array_t&) const { return type_t::ARRAY; }
  };

 public:
  template <typename T>
  json_node_t(T& t) : _any{json_any_ref_t{t}} {}

  template <typename T>
  json_node_t(T const& t) : _any{json_any_cref_t{t}} {}

  json_node_t(bool b) : _any{b} {}

  template <typename T>
  static json_node_t make_json_node(T const&);

  template <typename T>
  static json_node_t make_json_node(T&);

  template <typename T>
  T const& value_as() const {
    assert(!_any.empty());

    if (_any.which() == 0) {
      auto& value = boost::get<json_any_ref_t>(_any);
      TRACE_(2, "json_node_t() value_as() any.which()=" << _any.which() << " const value.which()=" << value.which());
      return boost::get<T&>(value);
    } else if (_any.which() == 1) {
      auto& value = boost::get<json_any_cref_t>(_any);
      TRACE_(2, "json_node_t() value_as() const any.which()=" << _any.which()
                                                              << " const value.which()=" << value.which());
      return boost::get<T const&>(value);
    }

    throw cet::exception("json_node_t") << "Value was not set";
  }

  template <typename T>
  T& value_as() {
    assert(!_any.empty());
    assert(_any.which() == 0);
    auto& value = boost::get<json_any_ref_t>(_any);

    TRACE_(2, "json_node_t() value_as() any.which()=" << _any.which() << " value.which()=" << value.which());

    return boost::get<T&>(value);
  }

  template <typename T, typename O>
  T& makeChild(O const& o) {
    using namespace artdaq::database::sharedtypes;

    assert(type() == type_t::OBJECT || type() == type_t::ARRAY);

    auto const node_name = unwrap(o).value_as<const std::string>(literal::name);

    TRACE_(16, "json_node_t() makeChild() node_name=<" << node_name << ">");

    if (type() == type_t::OBJECT) {
      object_t& object = value_as<object_t>();
      object[node_name] = T{};

      return unwrap(object.at(node_name)).value_as<T>();
    } else if (type() == type_t::ARRAY) {
      array_t& array = value_as<array_t>();
      array.push_back(T{});

      return unwrap(array.back()).value_as<T>();
    }

    throw cet::exception("json_node_t") << "Value was not created";
  }

  template <typename T, typename O>
  T& makeChildOfChildren(O const& o) {
    using namespace artdaq::database::sharedtypes;

    assert(type() == type_t::OBJECT || type() == type_t::ARRAY);
    TRACE_(16, "json_node_t() makeChildOfChildren() node_name=<children>");

    object_t& object = value_as<object_t>();
    object[literal::children] = object_t();

    return json_node_t{unwrap(object.at(literal::children)).value_as<object_t>()}.makeChild<T, O>(o);
  }

  type_t type() const {
    assert(!_any.empty());

    if (_any.which() == 0)
      return boost::apply_visitor(type_visitor(), boost::get<json_any_ref_t>(_any));
    else if (_any.which() == 1)
      return boost::apply_visitor(type_visitor(), boost::get<json_any_cref_t>(_any));
    else {
      return type_t::NOTSET;
    }
  }

  // defaults
  ~json_node_t() = default;
  json_node_t(json_node_t&&) = default;
  json_node_t(json_node_t const&) = default;

  // deleted
  json_node_t() = delete;
  json_node_t& operator=(json_node_t const&) = delete;
  json_node_t& operator=(json_node_t&&) = delete;

 private:
  boost::variant<json_any_ref_t, json_any_cref_t, bool> _any;
};

template <typename T>
json_node_t json_node_t::make_json_node(T& t) {
  return {t};
}

template <typename T>
json_node_t json_node_t::make_json_node(T const& t) {
  return {t};
}

class db2gui final {
 public:
  explicit db2gui(json_node_t data_node, json_node_t metadata_node)
      : _data_node{std::move(data_node)}, _metadata_node{std::move(metadata_node)} {}

  void operator()(json_node_t&) const;

 private:
  json_node_t _data_node;
  json_node_t _metadata_node;
};

class gui2db final {
 public:
  explicit gui2db(json_node_t gui_node) : _gui_node{std::move(gui_node)} {}

  void operator()(json_node_t&, json_node_t&) const;

 private:
  json_node_t _gui_node;
};

class data_node_t final {
 public:
  explicit data_node_t(json_node_t const& node) : _node{node} {}

  template <typename T>
  T const& value() const {
    return _node.value_as<T>();
  }

  type_t type() const { return _node.type(); }

 private:
  json_node_t const& _node;
};

class metadata_node_t final {
 public:
  explicit metadata_node_t(json_node_t const& node) : _node{node} {}

  std::string typeName() const { return _string_value_of(literal::type); }
  std::string comment() const { return _string_value_of(literal::comment); }
  std::string annotation() const { return _string_value_of(literal::annotation); }

  bool hasMetadata() const;

  json_node_t child(std::string const&) const;

 private:
  std::string _string_value_of(std::string const&) const;

 private:
  json_node_t const& _node;
};

class gui_node_t final {
 public:
  explicit gui_node_t(json_node_t const& node) : _node{node} {}

  template <typename T>
  T const& value() const {
    return _node.value_as<T>();
  }

  type_t type() const { return _node.type(); }

 private:
  json_node_t const& _node;
};

template <typename A>
struct jcunwrapper {
  jcunwrapper(A& a) : any{a} {}

  operator object_t const&() { return boost::get<object_t const&>(any); }

  operator array_t const&() { return boost::get<array_t const&>(any); }

  operator data_t const&() { return boost::get<data_t const&>(any); }

  operator value_t const&() { return boost::get<value_t const&>(any); }

  json_any_cref_t any;
};

template <typename A>
jcunwrapper<A> jcunwrap(A& any) {
  return jcunwrapper<A>(any);
}

void trace_enable_convertjson2guijson();

}  // namespace fhicljson
bool json_db_to_gui(std::string const&, std::string&);
bool json_gui_to_db(std::string const&, std::string&);
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_FHICLJSON_CONVERTJSON2JSONGUI_H_ */
