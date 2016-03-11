#include "artdaq-database/FhiclJson/common.h"

#include "artdaq-database/FhiclJson/fhicl_reader.h"

#include "artdaq-database/FhiclJson/fhicl_types.h"
#include "artdaq-database/FhiclJson/json_types.h"
#include "artdaq-database/FhiclJson/shared_literals.h"
#include "artdaq-database/FhiclJson/healper_functions.h"
#include "artdaq-database/FhiclJson/convertfhicljsongui.h"
#include "artdaq-database/FhiclJson/convertfhicljsondb.h"

#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/parse.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "FCL:FclReader_C"

namespace jsn = artdaq::database::json;

using artdaq::database::fhicl::FhiclReader;

bool FhiclReader::read_data_db(std::string const& in, jsn::object_t& json_object)
{
    assert(!in.empty());
    assert(json_object.empty());
    
    TRACE_(2, "read_data_db() begin");

    try {
        using boost::spirit::qi::phrase_parse;
        using boost::spirit::qi::blank;

        using artdaq::database::fhicl::fhicl_comments_parser_grammar;
        using artdaq::database::fhicl::pos_iterator_t;	
	using artdaq::database::fhicljson::fcl2jsondb;
	using artdaq::database::fhicljson::datapair_t;

        auto comments = comments_t();
        fhicl_comments_parser_grammar< pos_iterator_t > grammar;
        auto start = pos_iterator_t(in.begin());
        auto end = pos_iterator_t(in.end());

        if(!phrase_parse(start, end, grammar, blank, comments))
	  comments[0]="No comments";

        auto object = jsn::object_t();
	object[literal::data_node]=jsn::object_t();		
	object[literal::metadata_node]=jsn::object_t();	

	std::string masked_includes = std::regex_replace(in,std::regex(regex::parse_include), "//$1");
		
        ::fhicl::intermediate_table fhicl_table;
        ::fhicl::parse_document(masked_includes, fhicl_table);
	
	TRACE_(2, "parse_document() returned " << std::distance(fhicl_table.begin(),fhicl_table.end()) << " entries.");

        for (auto const & atom : fhicl_table){
	     datapair_t pair=std::move(fcl2jsondb(atom, atom, comments));
             boost::get<jsn::object_t>(object[literal::data_node]).push_back(std::move(pair.first));
             boost::get<jsn::object_t>(object[literal::metadata_node]).push_back(std::move(pair.second));
	}
	
        json_object.swap(object);

	TRACE_(2, "read_data_db() end");

        return true;

    } catch (::fhicl::exception const& e) {
      	TRACE_(2, "read_data_db() Caught fhicl::exception message=" << e.what());

        std::cerr << "Caught fhicl::exception message=" << e.what() << "\n";
        throw;
    }
}

bool FhiclReader::read_data_gui(std::string const& in, jsn::array_t& json_array){
    assert(!in.empty());
    assert(json_array.empty());

    try {
        using boost::spirit::qi::phrase_parse;
        using boost::spirit::qi::blank;

        using artdaq::database::fhicl::fhicl_comments_parser_grammar;
        using artdaq::database::fhicl::pos_iterator_t;
	using artdaq::database::fhicljson::fcl2jsongui;

        auto comments = comments_t();
        fhicl_comments_parser_grammar< pos_iterator_t > grammar;
        auto start = pos_iterator_t(in.begin());
        auto end = pos_iterator_t(in.end());

        if(!phrase_parse(start, end, grammar, blank, comments))
	  comments[0]="No comments";

        auto array = jsn::array_t();

	std::string masked_includes = std::regex_replace(in,std::regex(regex::parse_include), "//$1");
		
        ::fhicl::intermediate_table fhicl_table;
        ::fhicl::parse_document(masked_includes, fhicl_table);

        for (auto const & atom : fhicl_table)
            array.push_back(fcl2jsongui(atom, atom, comments));

        json_array.swap(array);

        return true;

    } catch (::fhicl::exception const& e) {
        std::cerr << "Caught fhicl::exception message=" << e.what() << "\n";
        throw;
    }
}

bool FhiclReader::read_comments(std::string const& in, jsn::array_t& json_array)
{
    assert(!in.empty());
    assert(json_array.empty());

    try {
        using boost::spirit::qi::phrase_parse;
        using boost::spirit::qi::blank;

        using artdaq::database::fhicl::fhicl_comments_parser_grammar;
        using artdaq::database::fhicl::pos_iterator_t;

        fhicl_comments_parser_grammar< pos_iterator_t > grammar;

        auto start = pos_iterator_t(in.begin());
        auto end = pos_iterator_t(in.end());

        auto comments = comments_t();

        auto result  = phrase_parse(start, end, grammar, blank, comments);
	
	if(!result) {
 	  comments[0]="No comments";
	  result =true;
	}
	
	auto array = jsn::array_t();
		
	for(auto const& comment : comments) {
	  array.push_back(jsn::object_t());
	  auto & object = boost::get<jsn::object_t>(array.back());
	  object.push_back(jsn::data_t::make(literal::linenum,comment.first));
	  object.push_back(jsn::data_t::make(literal::value, artdaq::database::fhicljson::filter_quotes(comment.second))); 
	}
	
        
        json_array.swap(array);

        return result;

    } catch (::fhicl::exception const& e) {
        std::cerr << "Caught fhicl::exception message=" << e.what() << "\n";
        throw;
    }
}



bool FhiclReader::read_includes(std::string const& in, jsn::array_t& json_array)
{
    assert(!in.empty());
    assert(json_array.empty());

    try {
        using boost::spirit::qi::phrase_parse;
        using boost::spirit::qi::blank;

        using artdaq::database::fhicl::fhicl_includes_parser_grammar;
        using artdaq::database::fhicl::pos_iterator_t;

        fhicl_includes_parser_grammar< pos_iterator_t > grammar;

        auto start = pos_iterator_t(in.begin());
        auto end = pos_iterator_t(in.end());

        auto includes = includes_t();

        auto result  = phrase_parse(start, end, grammar, blank, includes);

	auto array = jsn::array_t();
	
	for(auto const& include : includes)
	  array.push_back(include);
	
        if (result)
            json_array.swap(array);

        return result;

    } catch (::fhicl::exception const& e) {
        std::cerr << "Caught fhicl::exception message=" << e.what() << "\n";
        throw;
    }
}

void artdaq::database::fhicl::trace_enable_FhiclReader()
{
    TRACE_CNTL("name",    TRACE_NAME);
    TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
    TRACE_CNTL("modeM", 1LL);
    TRACE_CNTL("modeS", 1LL);

    TRACE_(0, "artdaq::database::fhicl::FhiclReader" << "trace_enable");
}

