#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Conf/conf_common.h"
#include "artdaq-database/DataFormats/Conf/convertconf2json.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "convertconf2json.cpp"

// TODO conf to json conversion code

void artdaq::database::confjson::debug::Conf2Json() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::conf2json trace_enable";
}
