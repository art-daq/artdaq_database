#include <boost/exception/diagnostic_information.hpp>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include  <cstdio>
#include "artdaq-database/BuildInfo/printStackTrace.h"
#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "boost/program_options.hpp"
#include "cetlib/coded_exception.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/FhiclJson/json_common.h"

namespace bpo = boost::program_options;

namespace db = artdaq::database;

void test_001 ();

int main(int argc, char* argv[]) try {
  debug::registerUngracefullExitHandlers();

  std::ostringstream descstr;
  descstr << argv[0] << " <-h <help>> ";

  bpo::options_description desc = descstr.str();

  desc.add_options()("help,h", "produce help message");

  bpo::variables_map vm;
  
  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error const& e) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": " << e.what() << "\n";
    return process_exit_code::INVALID_ARGUMENT;
  }

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return process_exit_code::HELP;
  }

  test_001();
  
  return process_exit_code::SUCCESS;

} catch (...) {
  std::cerr << "Process exited with error: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

void test_001 (){
  auto row_count=int{3};
  auto col_count=int{3};

  auto data = std::vector<std::vector<std::string>>();
 
  for (int i =0; i<row_count; ++i){
    auto subarray=std::vector<std::string>();
    for (int j =0; j<col_count; ++j){
      subarray.push_back(std::to_string(i+j));
    }
    data.push_back(subarray);
  }
  
  using namespace artdaq::database::json;
  
  auto name=std::string{"data"};
  auto docAST= object_t{};
  docAST[name]=array_t{};
  
  auto & dataArray=boost::get<array_t>(docAST.at(name));
  
  for (auto& subarray: data){
    auto subarrayAST = array_t{};
    for (auto& element: subarray){
      subarrayAST.push_back(element);
    }
    dataArray.push_back(subarrayAST);
  }
  
  auto json = std::string{};
  
  JsonWriter{}.write(docAST,json);
  
  std::cout << json;
}

