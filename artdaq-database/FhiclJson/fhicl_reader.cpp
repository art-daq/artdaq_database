#include "artdaq-database/FhiclJson/common.h"

#include "artdaq-database/FhiclJson/fhicl_types.h"
#include "artdaq-database/FhiclJson/json_types.h"
#include "artdaq-database/FhiclJson/shared_literals.h"


#include "artdaq-database/FhiclJson/fhicl_reader.h"

#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/parse.h"

#include <cmath>

#include  <boost/variant/get.hpp>

namespace jsn = artdaq::database::json;

using comments_t = artdaq::database::fhicl::comments_t;

std::string tag_as_string(fhicl::value_tag tag)
{
    switch (tag) {
    default: return "undefined";
    case fhicl::UNKNOWN: return "unknown";
    case fhicl::NIL: return "nil";
    case fhicl::STRING: return "string";
    case fhicl::BOOL: return "bool";
    case fhicl::NUMBER: return "number";
    case fhicl::COMPLEX: return "complex";
    case fhicl::SEQUENCE: return "sequence";
    case fhicl::TABLE: return "table";
    case fhicl::TABLEID: return "tableid";
    }
}

bool isDouble(std::string const& str)
{
    std::regex ex( {regex::parse_double});
    return std::regex_match(str, ex);
}

std::string filter_quotes(std::string const& str) 
{
  return std::regex_replace(str,std::regex("\""), "|");
}

struct fcl2json final {
    using fcl_value = fhicl::extended_value;
    using fhicl_key_value_pair_t = std::pair<std::string, fcl_value> const;

    explicit fcl2json(fhicl_key_value_pair_t const& self_,
                      fhicl_key_value_pair_t const& parent_,
                      comments_t const&             comments_)
        : self {self_}, parent {parent_}, comments {comments_} {};

    operator jsn::object_t()
    try {
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

        if (value.tag != fhicl::TABLE)
            add_comment(annotation_at, literal::annotation, linenum);


        switch (value.tag) {
        default:
            throw ::fhicl::exception(::fhicl::parse_error, literal::name) << ("Failure while parsing fcl node name=" + key);

        case fhicl::UNKNOWN:
        case fhicl::NIL:
        case fhicl::STRING: {
            object[literal::value] = dequote(fcl_value::atom_t(value));
            break;
        }
        case fhicl::BOOL: {
            bool boolean;
            std::istringstream(fcl_value::atom_t(value)) >> std::boolalpha >> boolean;
            object[literal::value] = boolean;
            break;
        }
        case fhicl::NUMBER: {
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
        case fhicl::COMPLEX: {
            object[literal::value] = dequote(fcl_value::atom_t(value));
            break;
        }
        case fhicl::SEQUENCE: {
            object[literal::values] = jsn::array_t();
            auto& tmpArray = boost::get<jsn::array_t>(object[literal::values]);

            tmpArray.reserve(fcl_value::sequence_t(value).size());

            for (auto const & tmpVal : fcl_value::sequence_t(value))
                tmpArray.push_back(dequote(fcl_value::atom_t(tmpVal)));

            break;
        }
        case fhicl::TABLE: {
            object[literal::children] = jsn::array_t();
            auto& tmpArray = boost::get<jsn::array_t>(object[literal::children]);

            tmpArray.reserve(fcl_value::table_t(value).size());

            for (auto const & kvp : fcl_value::table_t(value))
                tmpArray.push_back(fcl2json(kvp, self, comments));

            break;
        }

        case fhicl::TABLEID: {
            object[literal::value] = value.to_string();

            break;
        }
        }

        return object;
    } catch (boost::bad_lexical_cast const& e) {
        throw ::fhicl::exception(::fhicl::cant_insert, self.first) << e.what();
    } catch (boost::bad_numeric_cast const& e) {
        throw ::fhicl::exception(::fhicl::cant_insert, self.first) << e.what();
    } catch (fhicl::exception const& e) {
        throw ::fhicl::exception(::fhicl::cant_insert, self.first, e);
    } catch (std::exception const& e) {
        throw ::fhicl::exception(::fhicl::cant_insert, self.first) << e.what();
    }

    fhicl_key_value_pair_t const& self;
    fhicl_key_value_pair_t const& parent;
    comments_t const& comments;
};


using artdaq::database::fhicl::FhiclReader;

bool FhiclReader::read_data(std::string const& in, jsn::array_t& json_array)
{
    assert(!in.empty());
    assert(json_array.empty());

    try {
        using boost::spirit::qi::phrase_parse;
        using boost::spirit::qi::blank;

        using artdaq::database::fhicl::fhicl_comments_parser_grammar;
        using artdaq::database::fhicl::pos_iterator_t;

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
            array.push_back(fcl2json(atom, atom, comments));

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
    assert(comments.empty());

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
	array.reserve(comments.size());
		
	for(auto const& comment : comments) {
	  array.push_back(jsn::object_t());
	  auto & object = boost::get<jsn::object_t>(array.back());
	  object.push_back(jsn::data_t::make(literal::linenum,comment.first));
	  object.push_back(jsn::data_t::make(literal::value, filter_quotes(comment.second))); 
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
	array.reserve(includes.size());
	
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


