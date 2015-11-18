#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/fhicl_json.h"

#include "artdaq-database/FhiclJson/fhicl_reader.h"

#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/parse.h"

#include  <boost/variant/get.hpp>

using namespace artdaq::database;
using namespace fhicl;

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

struct fcl2json final {
    using ext_value = fhicl::extended_value;
    using fhicl_key_value_pair_t = std::pair<std::string, ext_value> const;
    
    explicit fcl2json(fhicl_key_value_pair_t const& self_, 
		      fhicl_key_value_pair_t const& parent_, 
		      comments_t const&             comments_)
        : self {self_}, parent {parent_}, comments {comments_} {};

    operator table_t()
    try {
        auto const& key   = self.first;
        auto const& value = self.second;

        assert(!key.empty());
        assert(!comments.empty());

        auto table = table_t();

        table[literal::type] = tag_as_string(value.tag);

        auto parse_linenum = [](std::string const& str) ->int {
	    if(str.empty())
	      return -1;
	    
            auto tmp = str.substr(str.find_last_of(":") + 1);
            return boost::lexical_cast<int>(tmp);
        };

        auto annotaion_at = [this](int linenum) ->std::string {
	    assert(linenum>-1);
	    
            if (comments.empty() || linenum<1)
                return literal::whitespace;

            auto search = comments.find(linenum);
            if (comments.end() == search)
                return literal::whitespace;

            return search->second;
        };

        auto comment_at = [&annotaion_at](int linenum) ->std::string {
	    assert(linenum>-1);
	    
            auto comment = annotaion_at(linenum);
            if (!comment.empty() && comment.at(0) != '/')
                return comment;

            return literal::whitespace;
        };

        auto add_comment = [&table](auto& func, std::string field, int linenum) {
	    assert(linenum>-1);
	    
            auto result = func(linenum);
	    
            if (!result.empty())
                table[field] = result;
        };
	
	auto dequote = [](auto s) {
	    if(s[0]=='"' && s[s.length()-1]=='"')
	      return s.substr(1,s.length()-2);
	    else
	      return s;
	  };

        auto linenum = parse_linenum(value.src_info);
	
        switch (value.tag) {
        case fhicl::UNKNOWN:
        case fhicl::NIL:
        case fhicl::BOOL:
        case fhicl::NUMBER: {
            table[literal::value] = ext_value::atom_t(value);
            add_comment(comment_at, literal::comment, linenum - 1);
            add_comment(annotaion_at, literal::annotaion, linenum);

            break;
        }

        case fhicl::STRING: {
            table[literal::value] = dequote(ext_value::atom_t(value));
            add_comment(comment_at, literal::comment, linenum - 1);
            add_comment(annotaion_at, literal::annotaion, linenum);

        }

        case fhicl::COMPLEX: {
            
            table[literal::value] = dequote(ext_value::atom_t(value));
            add_comment(comment_at, literal::comment, linenum - 1);
            add_comment(annotaion_at, literal::annotaion, linenum);

            break;
        }

        case fhicl::SEQUENCE: {
            table[literal::value] = fhicljson::sequence_t();

            auto& tmpSeq = boost::get<fhicljson::sequence_t>(table[literal::value]);

            tmpSeq.reserve(ext_value::sequence_t(value).size());

            for (auto const& tmpVal : ext_value::sequence_t(value))
                tmpSeq.push_back(dequote(ext_value::atom_t(tmpVal)));

            break;
        }
        case fhicl::TABLE: {
            table[literal::value] = fhicljson::table_t();

            auto& tmpTable =  boost::get<fhicljson::table_t>(table[literal::value]);

            for (auto const& kvp : ext_value::table_t(value))
                tmpTable[kvp.first] = fcl2json(kvp, self, comments);

            break;
        }
        default:
        case fhicl::TABLEID: {
            table[literal::value] = value.to_string();

            break;
        }
        }

        return table;
    } catch (boost::bad_lexical_cast const& e) {
        throw fhicl::exception(cant_insert, self.first) << e.what();
    } catch (boost::bad_numeric_cast const& e) {
        throw fhicl::exception(cant_insert, self.first) << e.what();
    } catch (fhicl::exception const& e) {
        throw fhicl::exception(cant_insert, self.first, e);
    } catch (std::exception const& e) {
        throw fhicl::exception(cant_insert, self.first) << e.what();
    }

    fhicl_key_value_pair_t const& self;
    fhicl_key_value_pair_t const& parent;
    comments_t const& comments;
};

bool FhiclReader::read(std::string const& in, table_t& outAst)
{
    assert(!in.empty());
    assert(outAst.empty());

    try {
        using boost::spirit::qi::phrase_parse;
        using boost::spirit::qi::blank;

        fhicl_comments_parser_grammar< pos_iterator_t > grammar;

        auto start = pos_iterator_t(in.begin());
        auto end = pos_iterator_t(in.end());

        auto comments = comments_t();

        if (!phrase_parse(start, end, grammar, blank, comments))
            throw fhicl::exception(parse_error, literal::comments_node) << "Failure while parsing fcl comments";

        auto tmpTable = table_t();

        fhicl::intermediate_table tbl;
        fhicl::parse_document(in, tbl);

        for (auto const& kvp : tbl)
            tmpTable[kvp.first] = fcl2json(kvp, kvp, comments);

        outAst.swap(tmpTable);

        return true;

    } catch (fhicl::exception const& e) {
        std::cerr << "Caught fhicl::exception message="<< e.what() << "\n";
	throw;
    }
}

bool FhiclReader::read_comments(std::string const& in, comments_t& comments)
{
    assert(!in.empty());
    assert(comments.empty());

    try {
        using boost::spirit::qi::phrase_parse;
        using boost::spirit::qi::blank;

        fhicl_comments_parser_grammar< pos_iterator_t > grammar;

        auto start = pos_iterator_t(in.begin());
        auto end = pos_iterator_t(in.end());

        auto buffer = comments_t();

        auto result  = phrase_parse(start, end, grammar, blank, buffer);

        if (result)
            comments.swap(buffer);

        return result;

    } catch (fhicl::exception const& e) {
        std::cerr << "Caught fhicl::exception message="<< e.what() << "\n";
	throw;
    }
}

