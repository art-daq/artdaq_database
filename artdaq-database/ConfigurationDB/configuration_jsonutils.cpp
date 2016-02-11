#ifndef _ARTDAQ_DATABASE_CONFIGURATION_JSONUTILS_H_
#define _ARTDAQ_DATABASE_CONFIGURATION_JSONUTILS_H_

#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/ConfigurationDB/configuration_jsonutils.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:ConfigJsonUtils_C"


namespace artdaq{
namespace database{
namespace configuration{
namespace jsonutils{
using artdaq::database::basictypes::JsonData;
  

std::vector<std::string> JSONDocument::split_path(std::string const& path)
{
  auto tmp = std::string{path};
  
  std::replace( tmp.begin(), tmp.end(), '.', ' ');
  
  std::vector<std::string> tokens{
      std::istream_iterator<std::string>{tmp},
      std::istream_iterator<std::string>{}};
      
  return  tokens;
}

		      
void JSONDocument::insertNode(std::string const& path, JsonData const& data)
{
     auto path_tockens=split_path(path);
     
     auto bson_document = bsoncxx::from_json(data.json_buffer);

     if (!bson_document)
        throw cet::exception("JSONDocument") << "Invalid data; json_buffer" << data.json_buffer;

     std::function<int (int)> factorial = [&] (int i) { 
    return (i == 1) ? 1 : i * factorial(i - 1); 
       
    }
     for(auto const& node : bson_document.view())
       
     
}

void JSONDocument::replaceNode(std::string const& path, JsonData const& data)
{
  
}

void JSONDocument::deleteNode(std::string const& path)
{
  
}

void JSONDocument::extractNode(std::string const& path, JsonData& data)
{
  
}
   
   
  void aa() {  
     auto bson_document = bsoncxx::from_json(data.json_buffer);

    if (!bson_document)
        throw cet::exception("MongoDB") << "Invalid data; json_buffer" << data.json_buffer;

    auto nested_document = [&bson_document](auto const & name) {
        auto view = bson_document->view();
        auto element = view.find(name);

        if (element == view.end())
            throw cet::exception("MongoDB") << "Search criteria is missing the \"" << name << "\" element.";

        return element->get_value();
    };

    auto collection_name = dequote(bsoncxx::to_json(nested_document("collection")));

    auto collection = _provider->connection().collection(collection_name);

    auto document = nested_document("document");

    auto filter = bbs::document {};

    auto isNew = bool {true};

    try {
        auto document_id = nested_document(collection_name + "_id");
        filter << (collection_name + "_id") << document_id;
	TRACE_( 4,   "search filter=" <<  bsoncxx::to_json(filter));

        isNew = false;
    } catch (cet::exception const&) {
	TRACE_( 4, "search filter is missing");
    }

    if (isNew) {
        auto insert = bbs::document {}
                      << (collection_name + "_id") << 0
                      << "document" << document << finalize;

        auto result [[gnu::unused]] =  collection.insert_one(insert);

        TRACE_(5, "inserted_id=" <<  bsoncxx::to_json(result->inserted_id()));

    } else {
	auto operation= std::string("$set");
	auto path = std::string("document");

	try {
	      operation = nested_document("update").nested_document("operation");
	      TRACE_( 6,   "update operation="<< operation);
	      path = nested_document("update").nested_document("path");
	      TRACE_( 6,   "update path="<< path);
	} catch (cet::exception const&ex) {
  	    TRACE_( 7,  "Caught an exception while reading the \"update\" element; ex.what()=" << ex.what());
	    operation= std::string("$set");
	    TRACE_( 6,   "update operation="<< operation);
	    path = std::string("document");
	    TRACE_( 6,   "update path="<< path);
	}
      
        auto update = bbs::document {};

        update <<  "$set" << open_document << "document" << document << close_document
               <<  "$currentDate" <<  open_document << "updated"
               <<  open_document << "$type" << "date" << close_document
               <<  close_document;

        auto result [[gnu::unused]] = collection.update_many(filter, update);

        TRACE_( 8,"modified_count=" << result->modified_count());
    }
  }
  
} //namespace jsonutils
} //namespace configuration
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATION_JSONUTILS_H_ */
