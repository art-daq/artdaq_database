#include "artdaq-database/BuildInfo/process_exceptions.h"
#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfl = cf::literal;
namespace cflp = cfl::provider;

void cf::validate_dbprovider_name(std::string const& provider) try{
  if (cf::not_equal(provider, cflp::filesystem) && cf::not_equal(provider, cflp::mongo) && cf::not_equal(provider, cflp::ucond))
    throw cet::exception("validate_dbprovider_name") << "Invalid database provider; database provider=" << provider << ".";
}catch (std::exception& ex) {
  TRACE_(0, "validate_dbprovider_name() Error:" << ex.what());
  throw;
}
