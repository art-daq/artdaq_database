#include "artdaq-database/FhiclJson/common.h"

#include "artdaq-database/FhiclJson/fhicl_types.h"
#include "artdaq-database/FhiclJson/json_types.h"
#include "artdaq-database/FhiclJson/shared_literals.h"

#include "artdaq-database/FhiclJson/fhicl_writer.h"

#include "fhiclcpp/exception.h"
#include "fhiclcpp/extended_value.h"


namespace fcl = artdaq::database::fhicl;
namespace jsn = artdaq::database::json;

fhicl::value_tag string_as_tag(std::string str)
{
    if (str == "nil")
        return fhicl::NIL;
    else if (str == "string")
        return fhicl::STRING;
    else if (str == "bool")
        return fhicl::BOOL;
    else if (str == "number")
        return fhicl::NUMBER;
    else if (str == "complex")
        return fhicl::COMPLEX;
    else if (str == "sequence")
        return fhicl::SEQUENCE;
    else if (str == "table")
        return fhicl::TABLE;
    else if (str == "tableid")
        return fhicl::TABLEID;

    throw fhicl::exception(fhicl::parse_error, literal::data_node) << ("FHiCL atom type \"" + str + "\" is not implemented.");
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

struct json2fcl final {

    explicit json2fcl(jsn::value_t const& self_,
                      jsn::value_t const& parent_)
        : self {self_}, parent {parent_} {};


    operator fcl::value_t()
    try {
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
    } catch (std::exception const&) {
        throw;
    }

    operator fcl::atom_t()
    try {
        if (self.type() != typeid(jsn::object_t))
            throw ::fhicl::exception(::fhicl::parse_error, literal::data_node) << ("JSON element is not a object_t type.");

        auto const& json_object = unwrap(self).value_as<const jsn::object_t>();

        auto type = string_as_tag(boost::get<std::string>(json_object.at(literal::type)));

        auto fcl_value = fcl::value_t();

        switch (type) {
        case fhicl::UNKNOWN:
        case fhicl::NIL:
        case fhicl::STRING: {
            fcl_value.value = boost::get<std::string>(json_object.at(literal::value));

            break;
        }

        case fhicl::BOOL: {
            fcl_value.value = boost::get<bool>(json_object.at(literal::value));
            break;
        }

        case fhicl::NUMBER: {
            if (json_object.at(literal::value).type() == typeid(int))
                fcl_value.value = boost::get<int>(json_object.at(literal::value));
            else
                fcl_value.value = boost::get<double>(json_object.at(literal::value));

            break;
        }

        case fhicl::COMPLEX: {
            fcl_value.value = boost::get<std::string>(json_object.at(literal::value));

            break;
        }

        case fhicl::SEQUENCE: {

            fcl_value.value = fcl::sequence_t();
            auto& sequence =  unwrap(fcl_value).value_as<fcl::sequence_t>();

            try {
                auto& values = boost::get<jsn::array_t>(json_object.at(literal::values));

                for (auto const & val : values)
                    sequence.push_back(json2fcl(val, self));

            } catch (std::out_of_range const&) {
            }

            break;
        }
        case fhicl::TABLE: {
            fcl_value.value = fcl::table_t();
            auto& table =  unwrap(fcl_value).value_as<fcl::table_t>();

            try {
                auto& values = boost::get<jsn::array_t>(json_object.at(literal::children));

                for (auto const & val : values)
                    table.push_back(json2fcl(val, self));

            } catch (std::out_of_range const&) {
            }

            break;
        }

        case fhicl::TABLEID: {
            fcl_value.value = boost::get<std::string>(json_object.at(literal::value));

            break;
        }
        }

        auto const& name = boost::get<std::string>(json_object.at(literal::name));
        auto const& comment = boost::get<std::string>(json_object.at(literal::comment));
        auto fcl_key = fcl::key_t(name, comment);

        if (type != fhicl::TABLE) //table does not have annotation
            fcl_value.annotation = boost::get<std::string>(json_object.at(literal::annotation));

        return  {fcl_key, fcl_value};

    } catch (std::exception const& e) {
        throw;
    }

    jsn::value_t const& self;
    jsn::value_t const& parent;
};

using artdaq::database::fhicl::FhiclWriter;

bool FhiclWriter::write_data(jsn::array_t const& json_array, std::string& out)
{
    assert(out.empty());
    assert(!json_array.empty());

    using artdaq::database::fhicl::fhicl_generator_grammar;

    auto result = bool(false);
    auto buffer = std::string();

    auto fhicl_table = fcl::table_t();

    for (auto const & json_value : json_array)
        fhicl_table.push_back(json2fcl(json_value, json_value));

    auto sink = std::back_insert_iterator<std::string>(buffer);

    fhicl_generator_grammar<decltype(sink)> grammar;

    result  = karma::generate(sink, grammar, fhicl_table);

    if (result)
        out.swap(buffer);

    return result;
}

std::string unescape(std::string const& str) 
{
  return std::regex_replace(str, std::regex("\\\""),"\"");
}

bool FhiclWriter::write_includes(jsn::array_t const& json_array, std::string& out)
{
    assert(out.empty());
    assert(!json_array.empty());

    using artdaq::database::fhicl::fhicl_include_generator_grammar;

    auto result = bool(false);
    auto buffer = std::string();
  
    auto includes = includes_t();
    
    includes.reserve(json_array.size());
 
    for (auto const & json_value : json_array)
        includes.push_back(unescape(boost::get<std::string>(json_value)));
	
    auto sink = std::back_insert_iterator<std::string>(buffer);

    fhicl_include_generator_grammar<decltype(sink)> grammar;

    result  = karma::generate(sink, grammar, includes);

    if (result)
        out.swap(buffer);

    return result;
}
