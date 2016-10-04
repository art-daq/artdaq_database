#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/storage_providers.h"

#include <wordexp.h>
#include <fstream> 
#include <algorithm> 

namespace db=artdaq::database;

std::string db::expand_environment_variables(std::string var) {
  wordexp_t p;
  char** w;

  ::wordexp(var.c_str(), &p, 0);

  w = p.we_wordv;

  std::stringstream ss;

  for (size_t i = 0; i < p.we_wordc; i++) ss << w[i] << "/";

  ::wordfree(&p);

  return ss.str();
}

db::object_id_t  db::generate_oid() {
  std::ifstream is("/proc/sys/kernel/random/uuid");

  std::string oid((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  oid.erase(std::remove(oid.begin(), oid.end(), '-'), oid.end());
  oid.resize(24);

  return oid;
}