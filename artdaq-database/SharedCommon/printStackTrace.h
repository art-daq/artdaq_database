#ifndef PRINTSTACKTRACE_H
#define PRINTSTACKTRACE_H

#include <string>


extern "C" {
  
#ifndef __clang__
  typedef void (__cxa_throw_t)(void*, void*, void (*)(void*));
  void __cxa_throw(void*, void*, void (*)(void*));
#else  //__clang__
  typedef __attribute__((noreturn)) void (__cxa_throw_t)(void *,std::type_info *,void(*)(void *));
  __attribute__((noreturn)) void __cxa_throw(void *,std::type_info *,void(*)(void *));
#endif //__clang__
}


namespace debug {
void registerAbortHandler();
void registerTerminateHandler();
void registerUncaughtExceptionHandler();
void registerUngracefullExitHandlers();
void trace_enable();

std::string current_exception_diagnostic_information();

std::string demangle(const char*);
std::string demangle(std::string const&);
std::string demangleStackTrace(void* const*, int);
std::string getCxaThrowStack();
std::string getStackTrace();
}

#endif /* #ifndef PRINTSTACKTRACE_H */
