#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/common/helper_functions.h"
#include "artdaq-database/DataFormats/common/shared_literals.h"

#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.h"
#include "artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h"
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"

#include <boost/variant/get.hpp>
#include <cmath>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "FHJS:fcl2jsondb_C"

namespace jsn = artdaq::database::json;
namespace fcl = artdaq::database::fhicl;

using comments_t = artdaq::database::fhicljson::comments_t;

using artdaq::database::fhicljson::fcl2jsondb;
using artdaq::database::fhicljson::json2fcldb;

namespace literal = artdaq::database::dataformats::literal;

std::string include("fhicl_pound_include");
std::string quoted_(std::string const& text) { return "\"" + text + "\""; }
bool need_quotes(std::string const& text) { return text.find_first_of(" .%()/") != std::string::npos; }

fcl2jsondb::fcl2jsondb(args_tuple_t args)
    : self{std::get<0>(args)}, parent{std::get<1>(args)}, comments{std::get<2>(args)}, opts{std::get<3>(args)} {}

fcl2jsondb::operator datapair_t() try {
  auto const& key = self.first;
  auto const& value = self.second;

  TRACE_(2, "fcl2jsondb() key=<" << key << ">");
  TRACE_(2, "fcl2jsondb() value=<" << value.to_string() << ">");

  using artdaq::database::fhicljson::return_pair;

  assert(!key.empty());
  assert(!comments.empty());

  auto returnValue = datapair_t();
  auto pair = return_pair{returnValue};

  pair.data.key = key;
  pair.metadata.key = key;
  pair.metadata.value = jsn::object_t();

  auto& object = pair.metadata_<jsn::object_t>();

  object[literal::type] = fcl::tag_as_string(value.tag);

  auto parse_linenum = [](std::string const& str) -> int {
    if (str.empty()) return -1;

    auto tmp = str.substr(str.find_last_of(":") + 1);
    return boost::lexical_cast<int>(tmp);
  };

  auto annotation_at = [this](int linenum) -> std::string {
    assert(linenum > -1);

    if (comments.empty() || linenum < 1) return literal::whitespace;

    auto search = comments.find(linenum);
    if (comments.end() == search) return literal::whitespace;

    return artdaq::database::dataformats::filter_jsonstring(search->second);
  };

  auto comment_at = [&annotation_at](int linenum) -> std::string {
    assert(linenum > -1);

    auto comment = annotation_at(linenum);
    if (!comment.empty() && comment.at(0) != '/') return comment;

    return literal::whitespace;
  };

  auto add_comment = [&object](auto& func, std::string field, int linenum) {
    assert(linenum > -1);

    auto result = func(linenum);

    if (!result.empty()) object[field] = result;
  };

  auto dequote = [](auto s) {
    if (s[0] == '"' && s[s.length() - 1] == '"')
      return s.substr(1, s.length() - 2);
    else
      return s;
  };

  auto linenum = parse_linenum(value.src_info);

  if (key.find("fhicl_pound_include_") != std::string::npos) {
    object[literal::comment] = std::string{literal::whitespace};
    object[literal::annotation] = std::string{literal::whitespace};
  } else {
    add_comment(comment_at, literal::comment, linenum - 1);

    if (value.tag != ::fhicl::TABLE) add_comment(annotation_at, literal::annotation, linenum);
  }

  TRACE_(2, "fcl2jsondb() value type=<" << fcl::tag_as_string(value.tag) << ">");

  switch (value.tag) {
    default:
      throw ::fhicl::exception(::fhicl::parse_error, literal::name) << ("Failure while parsing fcl node name=" + key);

    case ::fhicl::UNKNOWN:
    case ::fhicl::NIL:
    case ::fhicl::STRING: {
      pair.data.value = dequote(fcl_value::atom_t(value));
      break;
    }
    case ::fhicl::BOOL: {
      bool boolean;
      std::istringstream(fcl_value::atom_t(value)) >> std::boolalpha >> boolean;
      pair.data.value = boolean;
      break;
    }
    case ::fhicl::NUMBER: {
      std::string str = fcl_value::atom_t(value);

      if (fcl::isDouble(str)) {
        auto dbl = boost::lexical_cast<double>(str);

        if (std::fmod(dbl, static_cast<decltype(dbl)>(1.0)) == 0.0)
          pair.data.value = int(dbl);
        else
          pair.data.value = dbl;
      } else {
        pair.data.value = boost::lexical_cast<int>(str);
      }

      break;
    }
    case ::fhicl::COMPLEX: {
      pair.data.value = dequote(fcl_value::atom_t(value));
      break;
    }
    case ::fhicl::SEQUENCE: {
      pair.data.value = jsn::array_t();
      auto& tmpDataArray = pair.data_<jsn::array_t>();

      for (auto const& tmpVal : fcl_value::sequence_t(value)) {
        if (tmpVal.tag == ::fhicl::SEQUENCE) {
          tmpDataArray.push_back(jsn::array_t{});
          auto& subDataArray = boost::get<jsn::array_t>(tmpDataArray.back());
          for (auto const& subTmpVal : fcl_value::sequence_t(tmpVal)) {
            switch (subTmpVal.tag) {
              case ::fhicl::UNKNOWN:
              case ::fhicl::NIL:
              case ::fhicl::STRING: {
                subDataArray.push_back(dequote(fcl_value::atom_t(subTmpVal)));
                break;
              }
              case ::fhicl::BOOL: {
                bool boolean;
                std::istringstream(fcl_value::atom_t(subTmpVal)) >> std::boolalpha >> boolean;
                subDataArray.push_back(boolean);
                break;
              }
              case ::fhicl::NUMBER: {
                std::string str = fcl_value::atom_t(subTmpVal);

                if (fcl::isDouble(str)) {
                  auto dbl = boost::lexical_cast<double>(str);

                  if (std::fmod(dbl, static_cast<decltype(dbl)>(1.0)) == 0.0)
                    subDataArray.push_back(int(dbl));
                  else
                    subDataArray.push_back(dbl);
                } else {
                  subDataArray.push_back(boost::lexical_cast<int>(str));
                }
                break;
              }
              case ::fhicl::COMPLEX: {
                subDataArray.push_back(dequote(fcl_value::atom_t(subTmpVal)));
                break;
              }
              case ::fhicl::TABLEID:
              case ::fhicl::TABLE:
              case ::fhicl::SEQUENCE: {
                subDataArray.push_back(subTmpVal.to_string());
                break;
              }
            }
          }
        } else {
          tmpDataArray.push_back(dequote(fcl_value::atom_t(tmpVal)));
        }
      }

      break;
    }
    case ::fhicl::TABLE: {
      pair.data.value = jsn::object_t();
      pair.metadata_<jsn::object_t>()[literal::children] = jsn::object_t();

      auto& tmpDataObject = pair.data_<jsn::object_t>();
      auto& tmpMetadataObject = boost::get<jsn::object_t>(pair.metadata_<jsn::object_t>()[literal::children]);

      for (auto const& kvp : fcl_value::table_t(value)) {
        if ((kvp.second.in_prolog && opts.readProlog) || (!kvp.second.in_prolog && opts.readMain)) {
          datapair_t pair = std::move(fcl2jsondb(std::make_tuple(kvp, self, comments, opts)));
          tmpDataObject.push_back(std::move(pair.first));
          tmpMetadataObject.push_back(std::move(pair.second));
        }
      }

      break;
    }

    case ::fhicl::TABLEID: {
      pair.data.value = value.to_string();

      break;
    }
  }

  return returnValue;
} catch (boost::bad_lexical_cast const& e) {
  throw ::fhicl::exception(::fhicl::cant_insert, self.first) << e.what();
} catch (boost::bad_numeric_cast const& e) {
  throw ::fhicl::exception(::fhicl::cant_insert, self.first) << e.what();
} catch (::fhicl::exception const& e) {
  throw ::fhicl::exception(::fhicl::cant_insert, self.first, e);
} catch (std::exception const& e) {
  throw ::fhicl::exception(::fhicl::cant_insert, self.first) << e.what();
}

json2fcldb::json2fcldb(args_tuple_t args)
    : self{std::get<0>(args)}, parent{std::get<1>(args)}, opts{std::get<2>(args)} {}

json2fcldb::operator fcl::value_t() try {
  auto const& self_value = std::get<1>(self);

  if (self_value.type() == typeid(bool)) {
    return fcl::value_t(unwrap(self_value).value_as<const bool>());
  } else if (self_value.type() == typeid(int)) {
    return fcl::value_t(unwrap(self_value).value_as<const int>());
  } else if (self_value.type() == typeid(double)) {
    return fcl::value_t(unwrap(self_value).value_as<const double>());
  } else if (self_value.type() == typeid(std::string)) {
    return fcl::value_t(unwrap(self_value).value_as<const std::string>());
  }

  return fcl::value_t(literal::unknown);
} catch (std::exception const&) {
  throw;
}

json2fcldb::operator fcl::atom_t() try {
  auto const& self_key = std::get<0>(self);
  auto const& self_data = std::get<1>(self);
  auto const& self_metadata = std::get<2>(self);

  auto const& metadata_object = unwrap(self_metadata).value_as<const jsn::object_t>();

  auto type_name = boost::get<std::string>(metadata_object.at(literal::type));

  TRACE_(2, "json2fcldb() key=<" << self_key << ">"
                                 << " type=<" << type_name << ">");

  auto override_comment = false;

  if (type_name == "table" && self_data.type() == typeid(std::string)) {
    TRACE_(2, "json2fcldb() type override to string");
    type_name = "string";
    override_comment = true;
  }

  if (self_key.compare(0, include.length(), include) == 0) {
    TRACE_(2, "json2fcldb() name override to #include");

    auto fcl_key = fcl::key_t("#include", " ");
    auto fcl_value = fcl::value_t();

    fcl_value.value = quoted_(boost::get<std::string>(self_data));

    return {fcl_key, fcl_value};
  }

  auto type = fcl::string_as_tag(type_name);

  auto fcl_value = fcl::value_t();

  switch (type) {
    case ::fhicl::UNKNOWN:
    case ::fhicl::NIL:
    case ::fhicl::STRING: {
      auto value = boost::get<std::string>(self_data);
      fcl_value.value = need_quotes(value) ? quoted_(value) : value;

      break;
    }

    case ::fhicl::BOOL: {
      fcl_value.value = boost::get<bool>(self_data);
      break;
    }

    case ::fhicl::NUMBER: {
      if (self_data.type() == typeid(int))
        fcl_value.value = boost::get<int>(self_data);
      else
        fcl_value.value = boost::get<double>(self_data);

      break;
    }

    case ::fhicl::COMPLEX: {
      fcl_value.value = boost::get<std::string>(self_data);

      break;
    }

    case ::fhicl::SEQUENCE: {
      fcl_value.value = fcl::sequence_t();
      auto& sequence = unwrap(fcl_value).value_as<fcl::sequence_t>();

      try {
        auto const& values = boost::get<jsn::array_t>(self_data);

        for (auto const& tmpVal : values) {
          if (tmpVal.type() == typeid(jsn::array_t)) {
            sequence.push_back(fcl::value_t(fcl::sequence_t()));

            auto& sub_sequence = unwrap(sequence.back()).value_as<fcl::sequence_t>();

            auto const& sub_values = boost::get<jsn::array_t>(tmpVal);

            for (auto const& subTmpVal : sub_values) {
              if (subTmpVal.type() == typeid(std::string)) {
                sub_sequence.push_back(fcl::value_t(boost::get<std::string>(subTmpVal)));
              } else if (subTmpVal.type() == typeid(bool)) {
                sub_sequence.push_back(fcl::value_t(boost::get<bool>(subTmpVal)));
              } else if (subTmpVal.type() == typeid(int)) {
                sub_sequence.push_back(fcl::value_t(boost::get<int>(subTmpVal)));
              } else if (subTmpVal.type() == typeid(double)) {
                sub_sequence.push_back(fcl::value_t(boost::get<double>(subTmpVal)));
              }
            }
          } else {
            valuetuple_t value_tuple = std::forward_as_tuple(self_key, tmpVal, self_metadata);
            sequence.push_back(json2fcldb(std::make_tuple(value_tuple, self, opts)));
          }
        }
      } catch (std::out_of_range const&) {
      }

      break;
    }
    case ::fhicl::TABLE: {
      fcl_value.value = fcl::table_t();
      auto& table = unwrap(fcl_value).value_as<fcl::table_t>();

      try {
        auto const& object = boost::get<jsn::object_t>(self_data);
        auto const& children = boost::get<jsn::object_t>(metadata_object.at(literal::children));
        for (auto const& data : object) {
          valuetuple_t value_tuple = std::forward_as_tuple(data.key, data.value, children.at(data.key));

          table.push_back(json2fcldb(std::make_tuple(value_tuple, self, opts)));
        }

      } catch (std::out_of_range const&) {
      }

      break;
    }

    case ::fhicl::TABLEID: {
      fcl_value.value = boost::get<std::string>(self_data);

      break;
    }
  }

  auto const& name = self_key;
  auto const& comment = boost::get<std::string>(metadata_object.at(literal::comment));

  auto fcl_key = fcl::key_t(name, comment);

  if (type != ::fhicl::TABLE && !override_comment)  // table does not have annotation
    fcl_value.annotation = boost::get<std::string>(metadata_object.at(literal::annotation));

  return {fcl_key, fcl_value};

} catch (std::exception const& e) {
  throw;
}

using artdaq::database::sharedtypes::unwrap;
using artdaq::database::sharedtypes::unwrapper;

template <>
template <typename T>
T& unwrapper<jsn::value_t>::value_as() {
  return boost::get<T>(any);
}

template <>
template <typename T>
T const& unwrapper<const jsn::value_t>::value_as() {
  using V = typename std::remove_const<T>::type;

  return boost::get<V>(any);
}

template <>
template <typename T>
T& unwrapper<fcl::value_t>::value_as() {
  return boost::get<T>(any.value);
}

void artdaq::database::fhicljson::debug::enableFCL2JSON() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeM", trace_mode::modeM);

  TRACE_(0, "artdaq::database::convertjson::FCL2JSON trace_enable");
}
