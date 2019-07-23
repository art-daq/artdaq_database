#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/StorageProviders/MongoDB/mongo_json.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"
#include "artdaq-database/StorageProviders/common.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/SharedCommon/sharedcommon_common.h"

#include <bsoncxx/builder/basic/helpers.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/pipeline.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "mongo_functions.cpp"

using namespace artdaq::database;

bsoncxx::types::value extract_value_from_document(bsoncxx::document::value const& document, std::string const& key) {
  auto view = document.view();
  auto element = view.find(key);

  if (element == view.end()) {
    throw runtime_error("MongoDB") << "Search JSONDocument is missing the \"" << key << "\" element.";
  }

  return element->get_value();
}

mongocxx::pipeline pipeline_from_document(bsoncxx::document::view_or_value const& document ){
	mongocxx::pipeline pipeline;
  confirm(!document.is_owning());
 	auto view = document.view();
	confirm(!view.empty());

	for(auto const& kvp: view){
	 auto const & key=  kvp.key().to_string();
   auto document=[&kvp](){ auto doc= bsoncxx::builder::core(false); doc.concatenate(kvp.get_document());return doc; };

	 if(key=="$match"){
		 pipeline.match(document().view_document());
	 }else if (key == "$project") {
		 pipeline.project(document().view_document()); 
	 }

	}

  return pipeline;
}
