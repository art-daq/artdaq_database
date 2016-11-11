#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Bin/convertbin2json.h"
#include "artdaq-database/DataFormats/Bin/bin_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "BIN:bin2json_C"

// TODO bin to json conversion code

void artdaq::database::binjson::debug::enableBin2Json() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::bin2json trace_enable");
}
