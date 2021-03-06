#ifndef _ARTDAQ_DATABASE_DOCRECORD_JSONDOCUMENTMIGRATOR_H_
#define _ARTDAQ_DATABASE_DOCRECORD_JSONDOCUMENTMIGRATOR_H_

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/common.h"

namespace artdaq {
namespace database {
namespace docrecord {

class JSONDocumentMigrator final {
 public:
  JSONDocumentMigrator(JSONDocument&);
  operator JSONDocument();

  // defaults
  ~JSONDocumentMigrator() = default;

  // deleted
  JSONDocumentMigrator() = delete;
  JSONDocumentMigrator(JSONDocumentMigrator const&) = delete;
  JSONDocumentMigrator& operator=(JSONDocumentMigrator const&) = delete;
  JSONDocumentMigrator& operator=(JSONDocumentMigrator&&) = delete;
  JSONDocumentMigrator(JSONDocumentMigrator&&) = delete;

 private:
  JSONDocument& _document;
};

namespace debug {
void JSONDocumentMigrator();
}

}  // namespace docrecord
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DOCRECORD_JSONDOCUMENTMIGRATOR_H_ */
