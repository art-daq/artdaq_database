#ifndef _PROCESS_EXIT_CODES_H_
#define _PROCESS_EXIT_CODES_H_

namespace process_exit_code {
constexpr int INVALID_ARGUMENT = 128;
constexpr int UNCAUGHT_EXCEPTION = 144;
constexpr int SUCCESS = 0;
constexpr int FAILURE = 1;
constexpr int HELP = 1;
}

namespace trace_mode {
constexpr auto modeM = 1LL;

#ifndef NDEBUG
constexpr auto modeS = 1LL;
#else
constexpr auto modeS = 1LL;
#endif
}

#endif  // _PROCESS_EXIT_CODES_H_
