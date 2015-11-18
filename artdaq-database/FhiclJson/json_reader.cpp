#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/json_reader.h"

using namespace boost::spirit;
using namespace artdaq::database;

bool JsonReader::read(std::string const& in, any_value_t& ast)
{
    assert(!in.empty());
    assert(ast.empty());

    auto result = bool(false);
    any_value_t buffer;

    json_parser_grammar< std::string::const_iterator > grammar;

    std::string::const_iterator start = in.begin();

    result  = qi::phrase_parse(start, in.end(), grammar, ascii::space, buffer);

    if (result)
        ast.swap(buffer);

    return result;
}

