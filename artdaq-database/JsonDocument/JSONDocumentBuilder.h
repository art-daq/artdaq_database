#ifndef _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENTBUILDER_H_
#define _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENTBUILDER_H_

#include "artdaq-database/JsonDocument/common.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

namespace artdaq{
namespace database{
namespace jsonutils{
    
class JSONDocumentBuilder final
{
public:
   JSONDocumentBuilder();
   JSONDocumentBuilder(JSONDocument const&);
   
   JSONDocumentBuilder& createFromData(JSONDocument const&);  
   JSONDocumentBuilder& addAlias(JSONDocument const&);
   JSONDocumentBuilder& addToGlobalConfig(JSONDocument const&);
   JSONDocumentBuilder& setVersion(JSONDocument const&);
   JSONDocumentBuilder& removeAlias(JSONDocument const&);
   JSONDocumentBuilder& markReadonly();
   JSONDocumentBuilder& markDeleted();   
   
   void enforce() const;

   JSONDocument&  extract() {return _document;}
   
   //defaults    
   ~JSONDocumentBuilder() = default;
   JSONDocumentBuilder ( JSONDocumentBuilder && ) = default;
   
   //deleted
         
   JSONDocumentBuilder ( JSONDocumentBuilder const& ) = delete;
   JSONDocumentBuilder& operator= ( JSONDocumentBuilder const& ) = delete;
   JSONDocumentBuilder& operator= ( JSONDocumentBuilder && ) = delete;

private:
  void _createFromTemplate(JSONDocument document) {_document=std::move(document);}
  JSONDocument _makeActiveAlias(JSONDocument const&) const;
  JSONDocument _makeHistoryAlias(JSONDocument const&) const;
  JSONDocument _makeAlias(JSONDocument const&) const;
  JSONDocument _makeaddToGlobalConfig(JSONDocument const&) const;

  JSONDocument _makeReadonly() const;
  JSONDocument _makeDeleted() const;
  JSONDocument _wrap_as_payload(JSONDocument const&) const;
  JSONDocument _bookkeeping_update_payload(std::string) const;

   JSONDocumentBuilder& self(){return *this;}   
   JSONDocumentBuilder const& self() const{return *this;}
  
private:
  JSONDocument _document;
};
  

bool useFakeTime(bool );
std::string timestamp();


template <typename T> 
JSONDocument toJSONDocument(T const& t) {
  throw cet::exception("toJSONDocument") << "toJSONDocument is not implemented for type=<" << boost::demangle(typeid(t).name()) << ">";
};

void  trace_enable_JSONDocumentBuilder();
} //namespace jsonutils
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENTBUILDER_H_ */
