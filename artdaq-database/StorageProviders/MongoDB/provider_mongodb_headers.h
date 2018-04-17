#ifndef _ARTDAQ_DATABASE_PROVIDER_MONGO_HEADERS_H_
#define _ARTDAQ_DATABASE_PROVIDER_MONGO_HEADERS_H_

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

using namespace artdaq::database;
namespace db = artdaq::database;

namespace literal = db::mongo::literal;
namespace bbs = bsoncxx::builder::stream;

using bsoncxx::builder::concatenate_doc;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using artdaq::database::basictypes::JsonData;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::mongo::DBConfig;
using artdaq::database::mongo::MongoDB;

namespace jsonliteral = artdaq::database::dataformats::literal;

#endif /* _ARTDAQ_DATABASE_PROVIDER_MONGO_HEADERS_H_ */
