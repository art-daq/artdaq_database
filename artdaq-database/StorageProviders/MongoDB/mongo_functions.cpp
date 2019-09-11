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

mongocxx::pipeline pipeline_from_document(bsoncxx::document::value const& document) {
  mongocxx::pipeline pipeline;
  auto view = document.view();
  auto view_array = view.find("pipeline")->get_array().value;

  for (auto const& element : view_array) {
    const auto stage = bsoncxx::document::view{element.get_document()};

    auto length = std::distance(stage.cbegin(), stage.cend());
    if (length != 1) {
      throw runtime_error("MongoDB") << "Invalid MongoDB search; an aggregation pipeline must have only one"
                                     << " key-value-pair on each stage, stage=<" << compat::to_json(stage) << ">, length=" << length << ".";
    }

    auto const& kvp = *stage.begin();
    auto const key = std::string(kvp.key());

    auto document = [&kvp]() {
      auto doc = bsoncxx::builder::core(false);
      doc.concatenate(kvp.get_document());
      return doc;
    };

    if (key == "$match") {
      pipeline.match(document().view_document());
    } else if (key == "$project") {
      pipeline.project(document().view_document());
    } else if (key == "$group") {
      pipeline.group(document().view_document());
    } else if (key == "$sort") {
      pipeline.sort(document().view_document());
    } else if (key == "$addFields") {
      pipeline.add_fields(document().view_document());
    } else if (key == "$unwind") {
      pipeline.unwind(document().view_document());
    }
  }

  return pipeline;
}
