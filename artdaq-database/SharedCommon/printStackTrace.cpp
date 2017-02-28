#include <cxxabi.h>
#include <dlfcn.h>
#include <errno.h>
#include <execinfo.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>
#include <regex>
#include <string>
#include <typeinfo>

#include "artdaq-database/SharedCommon/common.h"
#include "artdaq-database/SharedCommon/printStackTrace.h"
#include "artdaq-database/SharedCommon/process_exit_codes.h"

#include <boost/exception/diagnostic_information.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "StackTrace_C"

namespace debug {
namespace stack {
void* last_frames[1024];
size_t last_size;
}
}

extern "C" {
void __cxa_throw(void* ex, void* info, void (*dest)(void*)) {
  using namespace debug::stack;
  last_size = backtrace(last_frames, sizeof last_frames / sizeof(void*));

  static void (*const rethrow)(void*, void*, void (*)(void*)) __attribute__((noreturn)) =
      (void (*)(void*, void*, void (*)(void*)))dlsym(RTLD_NEXT, "__cxa_throw");

  rethrow(ex, info, dest);
}
}

namespace debug {

std::string getStackTrace() {
  using namespace debug::stack;

  last_size = backtrace(last_frames, sizeof last_frames / sizeof(void*));

  std::ostringstream os;
  os << "Stack trace [" << last_size << " frames]:\n";
  if (last_size != 0) os << demangleStackTrace(last_frames, last_size);

  return os.str();
}

std::string getCxaThrowStack() {
  using namespace debug::stack;

  if (last_size == 0) return "None";

  std::ostringstream os;
  os << "Stack trace [" << last_size << " frames] @ __cxa_throw():\n";
  os << demangleStackTrace(last_frames, last_size) << "\n";

  return os.str();
}

std::string demangle(std::string const& name) { return debug::demangle(name.c_str()); }

std::string demangle(const char* name) {
  int status;
  std::unique_ptr<char, void (*)(void*)> realname(abi::__cxa_demangle(name, 0, 0, &status), &std::free);
  return status ? name : &*realname;
}

std::string demangleStackTrace(void* const* trace, int size) {
  using namespace debug::stack;

  char** symbols = backtrace_symbols(trace, size);
  std::ostringstream os;

  auto ex = std::regex{"[(](.*)[+]"};

  auto make_readable = [&ex](char* sym) {
    auto val = std::string{basename(sym)};
    auto m = std::smatch();

    if (std::regex_search(val, m, ex)) {
      if (m.size() > 1) val.replace(m.position(1), m.length(1), demangle(m[1]));
    }

    return val;
  };

  for (int i = size; i > 0; i--) os << std::setw(3) << i << " " << make_readable(symbols[i - 1]) << "\n";

  free(symbols);

  return os.str();
}

void signalHandler(int signum) {
  // aosociate each signal with a signal name string.
  const char* name = NULL;
  switch (signum) {
    case SIGABRT:
      name = "SIGABRT";
      break;
    case SIGSEGV:
      name = "SIGSEGV";
      break;
    case SIGBUS:
      name = "SIGBUS";
      break;
    case SIGILL:
      name = "SIGILL";
      break;
    case SIGFPE:
      name = "SIGFPE";
      break;
    case SIGTERM:
      name = "SIGTERM";
      break;
    case SIGQUIT:
      name = "SIGQUIT";
      break;
    case SIGSTKFLT:
      name = "SIGSTKFLT";
      break;
  }

  if (name) {
    TRACE_(1, "Caught signal " << signum << " (" << name << ")");
  } else {
    TRACE_(1, "Caught signal " << signum);
  }

  TRACE_(1, "" << getStackTrace());

  exit(signum);
}

void terminateHandler() {
  std::exception_ptr exptr = std::current_exception();
  if (exptr != 0) {
    auto pending = getCxaThrowStack();
    try {
      std::rethrow_exception(exptr);
    } catch (std::exception const& ex) {
      size_t funcnamesize = 1024;
      char funcname[1024];
      int status = 0;

      TRACE_(1, "Terminate called after throwing an instance of \'"
                    << abi::__cxa_demangle(typeid(ex).name(), funcname, &funcnamesize, &status) << "\'");

      TRACE_(1, " what(): " << ex.what());

      TRACE_(1, " details: " << pending);

    } catch (...) {
      TRACE_(1, "Terminate called after throwing an instance of unknown exception");
    }
  } else {
    TRACE_(1, "Terminate called");
  }

  TRACE_(1, getStackTrace());

  exit(SIGTERM);
}

void uncaughtExceptionHandler() {
  TRACE_(1, "Unexpected handler function called.");
  TRACE_(1, getCxaThrowStack());
  terminateHandler();
}

void registerAbortHandler() {
  signal(SIGABRT, signalHandler);
  signal(SIGSEGV, signalHandler);
  signal(SIGBUS, signalHandler);
  signal(SIGILL, signalHandler);
  signal(SIGFPE, signalHandler);
  signal(SIGQUIT, signalHandler);
  signal(SIGSTKFLT, signalHandler);

  std::set_terminate(terminateHandler);
}

void registerTerminateHandler() { std::set_terminate(terminateHandler); }

void registerUncaughtExceptionHandler() { std::set_unexpected(uncaughtExceptionHandler); }

void trace_enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);
}

void registerUngracefullExitHandlers() {
  trace_enable();
  registerAbortHandler();
  registerTerminateHandler();
  registerUncaughtExceptionHandler();
}

std::string current_exception_diagnostic_information() {
  std::ostringstream os;

  os << "Process exited";
  os << getCxaThrowStack();
  os << "Error: " << boost::current_exception_diagnostic_information();

  return os.str();
}
}