#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace apiliteral = artdaq::database::configapi::literal;

void cf::validate_dbprovider_name(std::string const& provider) try {
  if (db::not_equal(provider, apiliteral::provider::filesystem) &&
      db::not_equal(provider, apiliteral::provider::mongo) && db::not_equal(provider, apiliteral::provider::ucond))
    throw db::runtime_error("validate_dbprovider_name") << "Invalid database provider; database provider=" << provider
                                                        << ".";
} catch (std::exception& ex) {
  TRACE_(0, "validate_dbprovider_name() Error:" << ex.what());
  throw;
}
