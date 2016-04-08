#ifndef PRINTSTACKTRACE_H
#define PRINTSTACKTRACE_H

namespace debug {
void registerAbortHandler();
void registerTerminateHandler();
void registerUncaughtExceptionHandler();
void registerUngracefullExitHandlers();
void trace_enable();
}

#endif /* #ifndef PRINTSTACKTRACE_H */
