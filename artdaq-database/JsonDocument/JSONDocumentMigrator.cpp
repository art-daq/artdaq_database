#include "artdaq-database/JsonDocument/JSONDocumentMigrator.h"
#include "artdaq-database/JsonDocument/common.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSNU:DocuMgrt_C"

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::json::array_t;
using artdaq::database::json::type_t;
using artdaq::database::ThrowOnFailure;
using artdaq::database::result_t;

using artdaq::database::overlay::ovlDatabaseRecord;
using artdaq::database::overlay::ovlDatabaseRecordUPtr_t;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;

using namespace artdaq::database::docrecord;

namespace db = artdaq::database;
namespace utl = db::docrecord;
namespace ovl = db::overlay;

namespace dbdr = artdaq::database::docrecord;

namespace jsonliteral = artdaq::database::dataformats::literal;

JSONDocumentMigrator::JSONDocumentMigrator(JSONDocument const& document)
    : _document(document){}

JSONDocumentMigrator::operator JSONDocument() const {
  TRACE_(3, "JSONDocumentMigrator() begin");
  JSONDocumentBuilder builder{};
  

  builder.createFromData(_document);
  
  TRACE_(3, "JSONDocumentMigrator() end");
  return  builder.extract();
}
    

void dbdr::debug::JSONDocumentMigrator() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::JSONDocumentMigrator trace_enable");
}