#include "test/common.h"

#include "artdaq-database/ConfigurationDB/configurationdb.h"

int main(int argc [[gnu::unused]], char* argv[] [[gnu::unused]]) try {
  /*  auto json= "{\"aa\":1}";
    if (artdaq::database::docrecord::isValidJson(json))
      return process_exit_code::SUCCESS;
    */
  return process_exit_code::FAILURE;
} catch (...) {
  // std::cout << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
