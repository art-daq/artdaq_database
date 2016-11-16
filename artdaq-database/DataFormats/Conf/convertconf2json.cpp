#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Conf/convertconf2json.h"
#include "artdaq-database/DataFormats/Conf/conf_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CNF:conf2json_C"

// TODO conf to json conversion code

void artdaq::database::confjson::debug::enableConf2Json() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::conf2json trace_enable");
}
