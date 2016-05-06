#include "artdaq-database/XmlJson/common.h"

#include "artdaq-database/XmlJson/convertxml2json.h"
#include "artdaq-database/XmlJson/xml_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "XML:xml2json_C"

//TODO xml to json conversion code

void artdaq::database::xmljson::trace_enable_xml2json() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::xml2json trace_enable");
}
