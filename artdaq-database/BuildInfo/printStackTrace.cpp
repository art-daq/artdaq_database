#include <signal.h>
#include <stdio.h>
#include <cstdlib>

#include <cxxabi.h>
#include <errno.h>
#include <execinfo.h>

#include "trace.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "StackTrace_C"

void printStackTrace(FILE* out[[gnu::unused]] = stderr) {
  size_t trace_buffersize = 4096;
  char trace_buffer[trace_buffersize];

  TRACE(1, "Stack trace:");

  // storage array for stack trace address data
  void* addrlist[64];

  // retrieve current stack addresses
  unsigned int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

  if (addrlen == 0) {
    TRACE(1, " ");
    return;
  }

  // resolve addresses into strings containing "filename(function+address)",
  // Actually it will be ## program address function + offset
  // this array must be free()-ed
  char** symbollist = backtrace_symbols(addrlist, addrlen);

  size_t funcnamesize = 1024;
  char funcname[1024];

  // iterate over the returned symbol lines. skip the first, it is the
  // address of this function.
  for (unsigned int i = 4; i < addrlen; i++) {
    char* begin_name = NULL;
    char* begin_offset = NULL;
    char* end_offset = NULL;

    // find parentheses and +address offset surrounding the mangled name

    // ./module(function+0x15c) [0x8048a6d]
    for (char* p = symbollist[i]; *p; ++p) {
      if (*p == '(')
        begin_name = p;
      else if (*p == '+')
        begin_offset = p;
      else if (*p == ')' && (begin_offset || begin_name))
        end_offset = p;
    }

    if (begin_name && end_offset && (begin_name < end_offset)) {
      *begin_name++ = '\0';
      *end_offset++ = '\0';
      if (begin_offset) *begin_offset++ = '\0';

      // mangled name is now in [begin_name, begin_offset) and caller
      // offset in [begin_offset, end_offset). now apply
      // __cxa_demangle():

      int status = 0;
      char* ret = abi::__cxa_demangle(begin_name, funcname, &funcnamesize, &status);
      char* fname = begin_name;
      if (status == 0) fname = ret;

      if (begin_offset) {
        snprintf(trace_buffer, trace_buffersize, "  %-30s ( %-40s  + %-6s) %s\n", symbollist[i], fname, begin_offset,
                 end_offset);
      } else {
        snprintf(trace_buffer, trace_buffersize, "  %-30s ( %-40s    %-6s) %s\n", symbollist[i], fname, "", end_offset);
      }
    } else {
      // couldn't parse the line? print the whole line.
      snprintf(trace_buffer, trace_buffersize, "  %-40s\n", symbollist[i]);
    }
    TRACE(1, trace_buffer);
  }

  free(symbollist);
}

void signalHandler(int signum) {
  // associate each signal with a signal name string.
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

  // notify the user which signal was caught. We use printf, because this is the most
  // basic output function. Once you get a crash, it is possible that more complex output
  // systems like streams and the like may be corrupted. So we make the most basic call
  // possible to the lowest level, most standard print function.
  if (name) {
    TRACE_(1, "Caught signal " << signum << " (" << name << ")");
  } else {
    TRACE_(1, "Caught signal " << signum);
  }
  // Dump a stack trace.
  // This is the function we will be implementing next.
  printStackTrace();

  // If you caught one of the above signals, it is likely you just
  // want to quit your program right now.
  exit(signum);
}

void terminateHandler() {
  std::exception_ptr exptr = std::current_exception();
  if (exptr != 0) {
    try {
      std::rethrow_exception(exptr);
    } catch (std::exception const& ex) {
      size_t funcnamesize = 1024;
      char funcname[1024];
      int status = 0;

      TRACE_(1, "Terminate called after throwing an instance of \'"
                    << abi::__cxa_demangle(typeid(ex).name(), funcname, &funcnamesize, &status) << "\'");

      TRACE_(1, " what():" << ex.what());
    } catch (...) {
      TRACE_(1, "Terminate called after throwing an instance of unknown exception");
    }
  } else {
    TRACE_(1, "Terminate called");
  }

  printStackTrace();

  exit(SIGTERM);
}

void uncaughtExceptionHandler() { terminateHandler(); }

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

namespace debug {
void registerTerminateHandler() { std::set_terminate(terminateHandler); }

void registerUncaughtExceptionHandler() { std::set_unexpected(uncaughtExceptionHandler); }

void trace_enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);
}

void registerUngracefullExitHandlers() {
  trace_enable();
  registerAbortHandler();
  registerTerminateHandler();
  registerUncaughtExceptionHandler();
}
}
