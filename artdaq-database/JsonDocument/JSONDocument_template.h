#ifndef _ARTDAQ_DATABASE_JSONUTILS_DOCUMENT_TEMPLATE_H_
#define _ARTDAQ_DATABASE_JSONUTILS_DOCUMENT_TEMPLATE_H_

namespace artdaq{
namespace database{  
namespace jsonutils{

namespace literal
{
constexpr auto document       = "document";  
constexpr auto data           = "document.data";
constexpr auto search         = "document.search";
constexpr auto metadata       = "document.metadata";
constexpr auto comments       = "document.metadata.comments";

constexpr auto source         = "document.source";
constexpr auto source_rawdata = "document.source.rawdata";

constexpr auto version                = "version";

constexpr auto aliases                = "aliases";
constexpr auto aliases_active         = "aliases.active";
constexpr auto aliases_history        = "aliases.history";

constexpr auto bookkeeping            = "bookkeeping";
constexpr auto bookkeeping_updates    = "bookkeeping.updates";
constexpr auto bookkeeping_isdeleted  = "bookkeeping.isdeleted";
constexpr auto bookkeeping_isreadonly = "bookkeeping.isreadonly";

constexpr auto configurations = "configurations";
constexpr auto configurations_name           = "configurations.name";

}

namespace actions
{
constexpr auto addAlias       	= "addAlias";  
constexpr auto removeAlias    	= "removeAlias";
constexpr auto addToGlobalConfig= "addToGlobalConfig"; 
constexpr auto setVersion       = "setVersion";  
constexpr auto markDeleted      = "markDeleted";
constexpr auto markReadonly     = "markReadonly";

}

constexpr auto template__empty_document =
"{\n"
"    \"document\": \"empty\" \n"
"}";

constexpr auto template__default_document =
"{\n"
"    \"document\": {  \n"
"        \"data\": {\n"
"        },\n"
"\t\"metadata\": {\n"
"\t    \"comments\": { \"empty\":\"empty\"\n"
"\t\t}\n"
"\t    },\n"
"\t    \"search\": [\n"
"\t\t{\n"
"\t\t    \"name\": \"find any\",\n"
"\t\t    \"query\": \"not defined\"\n"
"\t\t}\n"
"\t    ]\n"
"\t},\n"
"\t\"source\": {\n"
"\t    \"encoding\": \"base64\",\n"
"\t    \"format\": \"json\",\n"
"\t    \"rawdata\": \"as\"\n"
"    },\n"
"    \n"
"    \"version\": \"default\",\n"
"    \"configurations\": [\n"
"    ],\n"
"    \"aliases\": {\n"
"        \"active\": [\n"
"        ],\n"
"        \"history\": [\n"
"        ]\n"
"    },\n"
"    \"bookkeeping\": {\n"
"        \"isdeleted\": false,\n"
"        \"isreadonly\": true,\n"
"        \"created\": \"Tue Dec 29 18:19:25 2015\",\n"
"        \"updates\": [\n"
"        ]\n"
"    }\n"
"}";

} //namespace jsonutils
} //namespace database
} //namespace artdaq


#endif /* _ARTDAQ_DATABASE_JSONUTILS_DOCUMENT_TEMPLATE_H_ */
