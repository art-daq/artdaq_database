#ifndef _ARTDAQ_DATABASE_PROVIDERS_H_
#define _ARTDAQ_DATABASE_PROVIDERS_H_

#include <list>
#include <memory>
#include <string>

#include "artdaq-database/SharedCommon/shared_datatypes.h"

namespace artdaq {
namespace database {

constexpr auto system_metadata = "SystemMetadata";

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
  std::list<TYPE> readDocument(FILTER const&);
  object_id_t writeDocument(TYPE const&);

  template <typename FILTER>
  std::list<FILTER> findConfigurations(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> addConfiguration(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> findVersions(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> findEntities(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> configurationComposition(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> listCollections(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> listDatabases(FILTER const&);

  template <typename FILTER>
  std::list<FILTER> databaseMetadata(FILTER const&);

 private:
  std::shared_ptr<IMPL> _provider;
};

constexpr auto ouid_invalid = "000000000000000000000000";

std::string make_database_metadata(std::string const& /*name*/, std::string const& /*uri*/);

}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_PROVIDERS_H_ */
