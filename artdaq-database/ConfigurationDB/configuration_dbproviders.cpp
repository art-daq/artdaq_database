#include "artdaq-database/BuildInfo/process_exceptions.h"
#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfl = cf::literal;
namespace cflp = cfl::provider;

void cf::validate_dbprovider_name(std::string const& provider) {
  if (cf::not_equal(provider, cflp::filesystem) && cf::not_equal(provider, cflp::mongo) &&
      cf::not_equal(provider, cflp::ucond)) {
    TRACE_(0, "Error: in   validate_dbprovider_name"
                  << " Invalid database provider; database provider=" << provider << ".");

    throw cet::exception("  validate_dbprovider_name") << "Invalid database provider; database provider=" << provider
                                                       << ".";
  }
}
