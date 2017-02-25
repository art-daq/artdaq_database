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
void test_002 ();

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

  test_002();
  
  return process_exit_code::SUCCESS;

} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

void test_001 (){
  std::size_t row_count =3;
  std::size_t col_count=3;

  auto data = std::vector<std::vector<std::string>>();
 
  for (std::size_t i =0; i<row_count; ++i){
    auto subarray=std::vector<std::string>();
    for (std::size_t j =0; j<col_count; ++j){
      subarray.push_back(std::to_string(i+j));
    }
    data.push_back(subarray);
  }
  
  using namespace artdaq::database::json;
  
  auto doc_name=std::string{"document"};
  auto data_name=std::string{"data"};
  auto dataset_name=std::string{"dataset"};
  
  auto docAST= object_t{};
  
  docAST[doc_name]=object_t{};
  auto & documentAST=boost::get<object_t>(docAST.at(doc_name));

  documentAST[data_name]=object_t{};
  auto & dataAST=boost::get<object_t>(documentAST.at(data_name));
  
  dataAST[dataset_name]=array_t{};
  auto & dataSetArray=boost::get<array_t>(dataAST.at(dataset_name));
  
  for (auto& subarray: data){
    auto subarrayAST = array_t{};
    for (auto& element: subarray){
      subarrayAST.push_back(element);
    }
    dataSetArray.push_back(subarrayAST);
  }
  
  auto json = std::string{};
  
  JsonWriter{}.write(docAST,json);
  
  std::cout << "test001\n"<< json << "\n";
  
}


void test_002 (){
  std::size_t row_count=7;

  auto table = std::vector<std::vector<std::string>>();
  auto cols = std::vector<std::string>{"name", "type", "status"};
 
  for (std::size_t i =0; i<row_count; ++i){
    auto subarray=std::vector<std::string>();
    for (std::size_t j =0; j<cols.size(); ++j){
      subarray.push_back(std::to_string(i+j));
    }
    table.push_back(subarray);
  }
  
  using namespace artdaq::database::json;
  
  auto doc_name=std::string{"document"};
  auto data_name=std::string{"data"};
  auto dataset_name=std::string{"dataset"};
  
  auto docAST= object_t{};
  
  docAST[doc_name]=object_t{};
  auto & documentAST=boost::get<object_t>(docAST.at(doc_name));

  documentAST[data_name]=object_t{};
  auto & dataAST=boost::get<object_t>(documentAST.at(data_name));
  
  dataAST[dataset_name]=array_t{};
  auto & dataSetArray=boost::get<array_t>(dataAST.at(dataset_name));

  
  for (auto& subarray: table){
    auto subarrayAST = object_t{};
    for (std::size_t j =0; j < cols.size(); ++j){
      subarrayAST[cols.at(j)]=subarray.at(j);
    }
    dataSetArray.push_back(subarrayAST);
  }
  
  auto json = std::string{};
  
  JsonWriter{}.write(docAST,json);
  
  std::cout << "test002\n"<< json << "\n";
}


