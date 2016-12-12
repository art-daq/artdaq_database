#ifndef PRINTSTACKTRACE_H
#define PRINTSTACKTRACE_H

#include <string>

extern "C" {
void __cxa_throw(void*, void*, void (*)(void*));
}

namespace debug {
void registerAbortHandler();
void registerTerminateHandler();
void registerUncaughtExceptionHandler();
void registerUngracefullExitHandlers();
void trace_enable();

std::string current_exception_diagnostic_information();

namespace stack {
void* last_frames[1024];
size_t last_size;
}

std::string demangle(const char*);
std::string demangle(std::string const&);
std::string demangleStackTrace(void* const*, int);
std::string getCxaThrowStack();
std::string getStackTrace();
}

#endif /* #ifndef PRINTSTACKTRACE_H */
