#ifndef _ARTDAQ_DATABASE_DOCRECORD_JSONDOCUMENTBUILDER_H_
#define _ARTDAQ_DATABASE_DOCRECORD_JSONDOCUMENTBUILDER_H_

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/common.h"
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"

namespace artdaq {
namespace database {
namespace docrecord {

using artdaq::database::json::array_t;
using artdaq::database::json::object_t;
using artdaq::database::json::type_t;
using artdaq::database::json::value_t;

using artdaq::database::overlay::ovlDatabaseRecord;
using artdaq::database::overlay::ovlDatabaseRecordUPtr_t;

class JSONDocumentBuilder final {
 public:
  JSONDocumentBuilder();
  JSONDocumentBuilder(JSONDocument);

  JSONDocumentBuilder& createFromData(JSONDocument);
  JSONDocumentBuilder& addAlias(JSONDocument const&);
  JSONDocumentBuilder& addConfiguration(JSONDocument const&);
  JSONDocumentBuilder& addEntity(JSONDocument const&);
  JSONDocumentBuilder& addRun(JSONDocument const&);

  JSONDocumentBuilder& setVersion(JSONDocument const&);
  JSONDocumentBuilder& setCollection(JSONDocument const&);

  JSONDocumentBuilder& removeAlias(JSONDocument const&);
  JSONDocumentBuilder& removeConfiguration(JSONDocument const&);
  JSONDocumentBuilder& removeEntity(JSONDocument const&);

  JSONDocumentBuilder& removeAllConfigurations();
  JSONDocumentBuilder& removeAllEntities();

  JSONDocumentBuilder& markReadonly();
  JSONDocumentBuilder& markDeleted();
  JSONDocumentBuilder& setObjectID(JSONDocument const&);

  bool newObjectID();
  bool isReadonlyOrDeleted() const;

  JSONDocument getObjectID() const;

  std::string getObjectOUID() const;

  std::list<std::string> extractTags() const;

  JSONDocument extract() { return std::move(_document); }

  result_t comapreUsingOverlays(JSONDocumentBuilder const&) const;

  result_t operator==(JSONDocumentBuilder const& other) const { return comapreUsingOverlays(other); };

  // defaults
  ~JSONDocumentBuilder() = default;

  // deleted
  JSONDocumentBuilder(JSONDocumentBuilder const&) = delete;
  JSONDocumentBuilder& operator=(JSONDocumentBuilder const&) = delete;
  JSONDocumentBuilder& operator=(JSONDocumentBuilder&&) = delete;
  JSONDocumentBuilder(JSONDocumentBuilder&&) = delete;

  std::string to_string() const;

 private:
  template <typename OVL>
  std::unique_ptr<OVL> overlay(JSONDocument&, object_t::key_type const&);
  void _createFromTemplate(JSONDocument document) { _document = std::move(document); }
  JSONDocumentBuilder& self() { return *this; }
  JSONDocumentBuilder const& self() const { return *this; }

  bool init();
  void _importUserData(JSONDocument const& document);

  result_t SaveUndo();
  result_t CallUndo() noexcept;

 private:
  JSONDocument _document;
  ovlDatabaseRecordUPtr_t _overlay;
  bool _initOK;
};

template <typename OVL>
std::unique_ptr<OVL> JSONDocumentBuilder::overlay(JSONDocument& document, object_t::key_type const& self_key) {
  confirm(!self_key.empty());
  confirm(type(document._value) == type_t::OBJECT);

  if (self_key.empty()) throw std::runtime_error("Errror: self_key is empty");

  if (type(document._value) != type_t::OBJECT) throw std::runtime_error("Errror: document._value is not a type_t::OBJECT type");

  using artdaq::database::sharedtypes::unwrap;

  return std::make_unique<OVL>(self_key, document._value);
}

template <typename T>
JSONDocument toJSONDocument(T const&);

namespace debug {
void JSONDocumentBuilder();
}

std::ostream& operator<<(std::ostream&, JSONDocumentBuilder const&);

}  // namespace docrecord
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DOCRECORD_JSONDOCUMENTBUILDER_H_ */
