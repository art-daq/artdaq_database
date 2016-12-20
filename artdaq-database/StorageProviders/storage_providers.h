#ifndef _ARTDAQ_DATABASE_PROVIDERS_H_
#define _ARTDAQ_DATABASE_PROVIDERS_H_

#include "artdaq-database/StorageProviders/common.h"

namespace artdaq {
namespace database {

using object_id_t = std::string;

namespace metadata {
constexpr auto system_metadata = "metadata";
}

template <typename TYPE, typename IMPL>
class StorageProvider final {
 public:
  using StorableType = TYPE;
  using Provider = StorageProvider<TYPE, IMPL>;
  using ProviderSPtr = std::shared_ptr<Provider>;

  static ProviderSPtr create(std::shared_ptr<IMPL> const& provider) {
    return std::make_shared<Provider, std::shared_ptr<IMPL> const&, PassKeyIdiom const&>(provider, {});
  }

  class PassKeyIdiom final {
   private:
    template <typename T, typename I>
    friend ProviderSPtr create(std::shared_ptr<I> const&);
    PassKeyIdiom() = default;
  };

  StorageProvider(std::shared_ptr<IMPL> const& provider, PassKeyIdiom const&) : _provider(provider) {}

  template <typename FILTER>
  std::list<TYPE> load(FILTER const&);
  object_id_t store(TYPE const&);

  template <typename FILTER>
  std::list<FILTER> findGlobalConfigs(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> addConfigToGlobalConfig(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> findConfigVersions(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> findConfigEntities(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> buildConfigSearchFilter(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> listCollectionNames(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> listDatabaseNames(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> databaseMetadata(FILTER const&);

 private:
  std::shared_ptr<IMPL> _provider;
};

constexpr auto ouid_invalid = "000000000000000000000000";

std::string expand_environment_variables(std::string /*var*/);
object_id_t generate_oid();
object_id_t extract_oid(std::string const&);
std::string make_database_metadata(std::string const& /*name*/, std::string const& /*uri*/);

}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_PROVIDERS_H_ */
