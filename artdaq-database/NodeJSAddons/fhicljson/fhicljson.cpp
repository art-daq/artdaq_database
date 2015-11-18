#include <fstream>
#include <streambuf>
#include  <sstream>

#include "fhicljson.h"
#include "artdaq-database/FhiclJson/fhicljson.h"
using namespace artdaq::database::fhicljson;

std::pair<bool, std::string> tojson(std::string const& file_name) {
  if(file_name.empty())
    return std::make_pair(false,"");
  
  std::ifstream is(file_name);
  
  std::stringstream fhicl_buffer;
  
  fhicl_buffer << is.rdbuf();
  is.close();
  
  auto fhicl=fhicl_buffer.str();
  auto json =std::string();
  
  auto result =fhicl_to_json(fhicl,json);
  
  return std::make_pair(result,json);
}

