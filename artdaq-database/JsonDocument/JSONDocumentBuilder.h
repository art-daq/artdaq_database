#ifndef _ARTDAQ_DATABASE_DOCRECORD_JSONDOCUMENTBUILDER_H_
#define _ARTDAQ_DATABASE_DOCRECORD_JSONDOCUMENTBUILDER_H_

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/common.h"
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"

namespace artdaq {
namespace database {
namespace docrecord {

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::json::array_t;
using artdaq::database::json::type_t;

using artdaq::database::overlay::ovlDatabaseRecord;
using artdaq::database::overlay::ovlDatabaseRecordUPtr_t;

class JSONDocumentBuilder final {
 public:
  JSONDocumentBuilder()
      : _document(std::string(template__empty_document)),
        _overlay(std::make_unique<ovlDatabaseRecord>(_document._value)) {}

  JSONDocumentBuilder(JSONDocument const& document)
      : _document(document), _overlay(std::make_unique<ovlDatabaseRecord>(_document._value)) {}

  JSONDocumentBuilder& createFromData(JSONDocument const&);
  JSONDocumentBuilder& addAlias(JSONDocument const&);
  JSONDocumentBuilder& addConfiguration(JSONDocument const&);

  JSONDocumentBuilder& setVersion(JSONDocument const&);
  JSONDocumentBuilder& setConfigurableEntity(JSONDocument const&);

  JSONDocumentBuilder& removeAlias(JSONDocument const&);
  JSONDocumentBuilder& removeConfiguration(JSONDocument const&);

  JSONDocumentBuilder& markReadonly();
  JSONDocumentBuilder& markDeleted();
  JSONDocumentBuilder& setObjectID(JSONDocument const&);

  JSONDocument& extract() { return _document; }

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

  void _importUserData(JSONDocument const& document);

  result_t SaveUndo();
  result_t CallUndo() noexcept;

 private:
  JSONDocument _document;
  ovlDatabaseRecordUPtr_t _overlay;
};

template <typename OVL>
std::unique_ptr<OVL> JSONDocumentBuilder::overlay(JSONDocument& document, object_t::key_type const& self_key) {
  confirm(!self_key.empty());
  confirm(type(document._value) == type_t::OBJECT);

  if (self_key.empty()) throw std::runtime_error("Errror: self_key is empty");

  if (type(document._value) != type_t::OBJECT)
    throw std::runtime_error("Errror: document._value is not a type_t::OBJECT type");

  using artdaq::database::sharedtypes::unwrap;

  return std::make_unique<OVL>(self_key, document._value);
}

template <typename T>
JSONDocument toJSONDocument(T const& t);

namespace debug {
void enableJSONDocumentBuilder();
}

std::ostream& operator<<(std::ostream&, JSONDocumentBuilder const&);

}  // namespace jsonrecord
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DOCRECORD_JSONDOCUMENTBUILDER_H_ */
