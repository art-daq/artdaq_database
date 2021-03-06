#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Xml/convertxml2json.h"
#include "artdaq-database/DataFormats/Xml/xml_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "convertxml2json.cpp"

// TODO(mu2etrg): xml to json conversion code

void artdaq::database::xmljson::debug::Xml2Json() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::xml2json trace_enable";
}
