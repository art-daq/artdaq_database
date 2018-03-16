#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace prov = artdaq::database::configapi::literal::provider;

void cf::validate_dbprovider_name(std::string const& provider) try {
  confirm(!provider.empty());

  if (db::not_equal(provider, prov::filesystem) && db::not_equal(provider, prov::mongo) &&
      db::not_equal(provider, prov::ucon))
    throw db::runtime_error("validate_dbprovider_name") << "Invalid database provider; database provider=" << provider
                                                        << ".";
} catch (std::exception& ex) {
  TLOG(10) <<  "validate_dbprovider_name() Error:" << ex.what();
  throw;
}
