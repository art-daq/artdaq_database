#include "artdaq-database/FhiclJson/common.h"

#include "artdaq-database/FhiclJson/fhicl_types.h"
#include "artdaq-database/FhiclJson/json_types.h"
#include "artdaq-database/FhiclJson/shared_literals.h"

#include "artdaq-database/FhiclJson/fhicl_writer.h"

#include "fhiclcpp/exception.h"
#include "fhiclcpp/extended_value.h"


namespace adf = artdaq::database::fhicl;
namespace adj = artdaq::database::json;

fhicl::value_tag string_as_tag(std::string str)
{
    if(str=="nil")
      return fhicl::NIL;
    else if(str=="string")
      return fhicl::STRING;
    else if(str=="bool")
      return fhicl::BOOL;
    else if(str=="number")
      return fhicl::NUMBER;
    else if(str=="complex")
      return fhicl::COMPLEX;
    else if(str=="sequence")
      return fhicl::SEQUENCE;
    else if(str=="table")
      return fhicl::TABLE;
    else if(str=="tableid")
      return fhicl::TABLEID;

    throw fhicl::exception(fhicl::parse_error, literal::data_node) << ("FHiCL atom type \"" + str + "\" is not implemented.");
}

struct json2fcl final {

    explicit json2fcl(adj::atom_t const& self_,
                      adj::atom_t const& parent_)
        : self {self_}, parent {parent_} {};

    operator adf::table_t()
    try {
	auto table = adf::table_t();
	
	auto const& self_key   = self.key;
	auto const& self_value = self.value;      
        
	//if(self_value.value.type()!=typeid(adj::table_t))
	//    throw ::fhicl::exception(::fhicl::parse_error, literal::data_node) << ("JSON element \"" + self_key.key + "\" is not a table_t type.");
	
	//auto const& json_table_value = unwarap_value_as<fhicljson::table_t>(self_value);
/*	
	table[self_key]=fhicljson::atom_t();
	
        auto& fhicl_table_value = table[self_key];

        auto copy_attribute_from_to = [](auto const& key) {
            return [ = ](auto const & from) {
                auto const& it = from.find(key);
                if (it != from.end()) {
                    auto const& value = it->second;
                    return [ = ](auto & to) {
                        to[key] = value;
                    };
                }
            };
        };
	
	copy_attribute_from_to(literal::annotation)(json_table_value)(fhicl_table_value);
	copy_attribute_from_to(literal::comment)(json_table_value)(fhicl_table_value);
	*/
	//assert();
	
	//auto const& current = 
	
	
        return table;
    } catch (std::exception const& e) {
        throw;
    }

    adj::atom_t const& self;
    adj::atom_t const& parent;
};

using artdaq::database::fhicl::FhiclWriter;

bool FhiclWriter::write(adj::table_t const& json_ast [[gnu::unused]], std::string& out)
{
    assert(out.empty());
    assert(!json_ast.empty());
       
    using artdaq::database::fhicl::fhicl_generator_grammar;

    auto result = bool(false);
    auto buffer = std::string();


    auto fhicl_ast = adf::table_t();

   
    //for (auto const & atom : json_ast)
    //    fhicl_ast[atom.key] = json2fcl(atom, atom);

    auto sink = std::back_insert_iterator<std::string>(buffer);

    fhicl_generator_grammar<decltype(sink)> grammar;

    result  = karma::generate(sink, grammar, fhicl_ast);

    if (result)
        out.swap(buffer);

    return result;
}

