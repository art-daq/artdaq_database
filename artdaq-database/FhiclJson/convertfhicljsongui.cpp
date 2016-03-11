#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/healper_functions.h"

#include "artdaq-database/FhiclJson/convertfhicljsongui.h"
#include "artdaq-database/FhiclJson/shared_literals.h"

#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/parse.h"

#include <cmath>

#include  <boost/variant/get.hpp>

namespace jsn = artdaq::database::json;

using comments_t = artdaq::database::fhicljson::comments_t;

using artdaq::database::fhicljson::fcl2jsongui;
using artdaq::database::fhicljson::json2fclgui;

fcl2jsongui::fcl2jsongui(fhicl_key_value_pair_t const& self_,
                         fhicl_key_value_pair_t const& parent_,
                         comments_t const&             comments_)
    : self {self_}, parent {parent_}, comments {comments_} {}

fcl2jsongui::operator jsn::object_t()
try
{
    auto const& key   = self.first;
    auto const& value = self.second;

    assert(!key.empty());
    assert(!comments.empty());

    auto object = jsn::object_t();

    object[literal::type] = tag_as_string(value.tag);
    object[literal::name] = key;

    auto parse_linenum = [](std::string const & str) ->int {
        if (str.empty())
            return -1;

        auto tmp = str.substr(str.find_last_of(":") + 1);
        return boost::lexical_cast<int>(tmp);
    };

    auto annotation_at = [this](int linenum) ->std::string {
        assert(linenum > -1);

        if (comments.empty() || linenum < 1)
            return literal::whitespace;

        auto search = comments.find(linenum);
        if (comments.end() == search)
            return literal::whitespace;

        return filter_quotes(search->second);
    };

    auto comment_at = [&annotation_at](int linenum) ->std::string {
        assert(linenum > -1);

        auto comment = annotation_at(linenum);
        if (!comment.empty() && comment.at(0) != '/')
            return comment;

        return literal::whitespace;
    };

    auto add_comment = [&object](auto & func, std::string field, int linenum) {
        assert(linenum > -1);

        auto result = func(linenum);

        if (!result.empty())
            object[field] = result;
    };

    auto dequote = [](auto s) {
        if (s[0] == '"' && s[s.length() - 1] == '"')
            return s.substr(1, s.length() - 2);
        else
            return s;
    };

    auto linenum = parse_linenum(value.src_info);

    add_comment(comment_at, literal::comment, linenum - 1);

    if (value.tag != ::fhicl::TABLE)
        add_comment(annotation_at, literal::annotation, linenum);


    switch (value.tag) {
    default:
        throw ::fhicl::exception(::fhicl::parse_error, literal::name) << ("Failure while parsing fcl node name=" + key);

    case ::fhicl::UNKNOWN:
    case ::fhicl::NIL:
    case ::fhicl::STRING: {
        object[literal::value] = dequote(fcl_value::atom_t(value));
        break;
    }
    case ::fhicl::BOOL: {
        bool boolean;
        std::istringstream(fcl_value::atom_t(value)) >> std::boolalpha >> boolean;
        object[literal::value] = boolean;
        break;
    }
    case ::fhicl::NUMBER: {
        std::string str = fcl_value::atom_t(value);

        if (isDouble(str)) {
            auto dbl = boost::lexical_cast<double>(str);

            if (std::fmod(dbl, static_cast<decltype(dbl)>(1.0)) == 0.0)
                object[literal::value] = int(dbl);
            else
                object[literal::value] = dbl;
        } else {
            object[literal::value] = boost::lexical_cast<int>(str);
        }

        break;
    }
    case ::fhicl::COMPLEX: {
        object[literal::value] = dequote(fcl_value::atom_t(value));
        break;
    }
    case ::fhicl::SEQUENCE: {
        object[literal::values] = jsn::array_t();
        auto& tmpArray = boost::get<jsn::array_t>(object[literal::values]);

        for (auto const & tmpVal : fcl_value::sequence_t(value))
            tmpArray.push_back(dequote(fcl_value::atom_t(tmpVal)));

        break;
    }
    case ::fhicl::TABLE: {
        object[literal::children] = jsn::array_t();
        auto& tmpArray = boost::get<jsn::array_t>(object[literal::children]);

        for (auto const & kvp : fcl_value::table_t(value))
            tmpArray.push_back(fcl2jsongui(kvp, self, comments));

        break;
    }

    case ::fhicl::TABLEID: {
        object[literal::value] = value.to_string();

        break;
    }
    }

    return object;
} catch (boost::bad_lexical_cast const& e)
{
    throw ::fhicl::exception(::fhicl::cant_insert, self.first) << e.what();
} catch (boost::bad_numeric_cast const& e)
{
    throw ::fhicl::exception(::fhicl::cant_insert, self.first) << e.what();
} catch (::fhicl::exception const& e)
{
    throw ::fhicl::exception(::fhicl::cant_insert, self.first, e);
} catch (std::exception const& e)
{
    throw ::fhicl::exception(::fhicl::cant_insert, self.first) << e.what();
}


json2fclgui::json2fclgui(jsn::value_t const& self_,
                         jsn::value_t const& parent_)
    : self {self_}, parent {parent_} {}


json2fclgui::operator fcl::value_t()
try
{
    if (self.type() == typeid(bool)) {
        return fcl::value_t(unwrap(self).value_as<const bool>());
    }  else if (self.type() == typeid(int)) {
        return fcl::value_t(unwrap(self).value_as<const int>());
    }  else if (self.type() == typeid(double)) {
        return fcl::value_t(unwrap(self).value_as<const double>());
    }  else if (self.type() == typeid(std::string)) {
        return fcl::value_t(unwrap(self).value_as<const std::string>());
    }

    return fcl::value_t(literal::unknown);
} catch (std::exception const&)
{
    throw;
}

json2fclgui::operator fcl::atom_t()
try
{
    if (self.type() != typeid(jsn::object_t))
        throw ::fhicl::exception(::fhicl::parse_error, literal::data_node) << ("JSON element is not a object_t type.");

    auto const& json_object = unwrap(self).value_as<const jsn::object_t>();

    auto type = string_as_tag(boost::get<std::string>(json_object.at(literal::type)));

    auto fcl_value = fcl::value_t();

    switch (type) {
      case ::fhicl::UNKNOWN:
      case ::fhicl::NIL:
      case ::fhicl::STRING: {
        fcl_value.value = boost::get<std::string>(json_object.at(literal::value));

        break;
    }

      case ::fhicl::BOOL: {
        fcl_value.value = boost::get<bool>(json_object.at(literal::value));
        break;
    }

      case ::fhicl::NUMBER: {
        if (json_object.at(literal::value).type() == typeid(int))
            fcl_value.value = boost::get<int>(json_object.at(literal::value));
        else
            fcl_value.value = boost::get<double>(json_object.at(literal::value));

        break;
    }

      case ::fhicl::COMPLEX: {
        fcl_value.value = boost::get<std::string>(json_object.at(literal::value));

        break;
    }

      case ::fhicl::SEQUENCE: {

        fcl_value.value = fcl::sequence_t();
        auto& sequence =  unwrap(fcl_value).value_as<fcl::sequence_t>();

        try {
            auto& values = boost::get<jsn::array_t>(json_object.at(literal::values));

            for (auto const & val : values)
                sequence.push_back(json2fclgui(val, self));

        } catch (std::out_of_range const&) {
        }

        break;
    }
      case ::fhicl::TABLE: {
        fcl_value.value = fcl::table_t();
        auto& table =  unwrap(fcl_value).value_as<fcl::table_t>();

        try {
            auto& values = boost::get<jsn::array_t>(json_object.at(literal::children));

            for (auto const & val : values)
                table.push_back(json2fclgui(val, self));

        } catch (std::out_of_range const&) {
        }

        break;
    }

      case ::fhicl::TABLEID: {
        fcl_value.value = boost::get<std::string>(json_object.at(literal::value));

        break;
    }
    }

    auto const& name = boost::get<std::string>(json_object.at(literal::name));
    auto const& comment = boost::get<std::string>(json_object.at(literal::comment));
    auto fcl_key = fcl::key_t(name, comment);

    if (type != ::fhicl::TABLE) //table does not have annotation
        fcl_value.annotation = boost::get<std::string>(json_object.at(literal::annotation));

    return  {fcl_key, fcl_value};

} catch (std::exception const& e)
{
    throw;
}


using artdaq::database::sharedtypes::unwrap;
using artdaq::database::sharedtypes::unwrapper;

template<>
template<typename T>
T&  unwrapper<jsn::value_t>::value_as()
{
    return boost::get<T>(any);
}

template<>
template<typename T>
T const&  unwrapper<const jsn::value_t>::value_as()
{
    using V = typename std::remove_const<T>::type;

    return boost::get<V>(any);
}

template<>
template<typename T>
T&  unwrapper<fcl::value_t>::value_as()
{
    return boost::get<T>(any.value);
}

