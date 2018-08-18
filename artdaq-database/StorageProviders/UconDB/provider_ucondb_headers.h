#ifndef _ARTDAQ_DATABASE_PROVIDER_UCOND_HEADERS_H_
#define _ARTDAQ_DATABASE_PROVIDER_UCOND_HEADERS_H_

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/SharedCommon/common.h"
#include "artdaq-database/SharedCommon/returned_result.h"
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb.h"
#include "artdaq-database/StorageProviders/UconDB/ucondb_api.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:UconDB_C"

namespace db = artdaq::database;
namespace dbuc = db::ucon;
namespace dbucl = dbuc::literal;
namespace jsonliteral = db::dataformats::literal;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::ucon::DBConfig;
using artdaq::database::ucon::UconDB;
using artdaq::database::ucon::UconDBSPtr_t;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;

#endif /* _ARTDAQ_DATABASE_PROVIDER_UCOND_HEADERS_H_ */
