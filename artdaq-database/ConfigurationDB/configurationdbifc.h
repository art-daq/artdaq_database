#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDBIFC_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDBIFC_H_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "artdaq-database/ConfigurationDB/configurationdbifc_base.h"
#pragma GCC diagnostic pop

#include "options_operation_manageconfigs.h"

using debug::demangle;

namespace artdaq {
namespace database {
namespace configuration {

namespace db = artdaq::database;
namespace jsonliteral = db::dataformats::literal;

namespace cf = db::configuration;
namespace impl = cf::opts;

using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::FhiclData;
using cf::options::data_format_t;

constexpr auto apiname = "ConfigurationInterface";

struct ConfigurationInterface final {
  using VersionInfoList_t = std::list<VersionInfo>;

  ConfigurationInterface(std::string const&){};

  //==============================================================================
  // stores configuration version to database
  template <typename CONF, typename TYPE>
  cf::result_t storeVersion(CONF configuration, std::string const& version, std::string const& entity) const
      noexcept try {
    confirm(!version.empty());

    constexpr auto apifunctname = "ConfigurationInterface::storeVersion";

    if (version.empty()) throw artdaq::database::invalid_option_exception(apifunctname) << "Version is empty";

    auto serializer = ConfigurationSerializer<CONF, MakeSerializable>::wrap(configuration);

    auto opts = ManageDocumentOperation{apiname};

    opts.operation(apiliteral::operation::writedocument);
    opts.collection(serializer.configurationName());
    opts.version(version);

    if (std::is_same<TYPE, JsonData>::value) {
      opts.format(data_format_t::json);
    } else if (std::is_same<TYPE, FhiclData>::value) {
      opts.format(data_format_t::fhicl);
    } else {
      throw artdaq::database::invalid_option_exception(apifunctname)
          << "Unsupported storage format " << demangle(typeid(TYPE).name()) << ",  use either JsonData or FhiclData.";
    }

    if (!entity.empty()) opts.entity(entity);

    auto data = TYPE{"{}"};

    auto writeResult = serializer.template writeDocument<TYPE>(data);

    if (!writeResult.first) throw artdaq::database::runtime_exception(apifunctname) << writeResult.second;

    std::ostringstream oss;
    oss << data;

    auto buffer = oss.str();

    auto apiCallResult = impl::write_document(opts, buffer);

    if (!apiCallResult.first) throw artdaq::database::runtime_exception(apifunctname) << apiCallResult.second;

    return apiCallResult;
  } catch (std::exception const& e) {
    return {false, {e.what()}};
  }

  //==============================================================================
  // loads configuration version from database
  template <typename CONF, typename TYPE>
  cf::result_t loadVersion(CONF configuration, std::string const& version, std::string const& entity) const
      noexcept try {
    confirm(!version.empty());

    constexpr auto apifunctname = "ConfigurationInterface::loadVersion";

    if (version.empty()) throw artdaq::database::invalid_option_exception(apifunctname) << "Version is empty";

    auto serializer = ConfigurationSerializer<CONF, MakeSerializable>::wrap(configuration);

    auto opts = ManageDocumentOperation{apiname};

    opts.operation(apiliteral::operation::readdocument);
    opts.collection(serializer.configurationName());
    opts.version(version);

    if (std::is_same<TYPE, JsonData>::value) {
      opts.format(data_format_t::json);
    } else if (std::is_same<TYPE, FhiclData>::value) {
      opts.format(data_format_t::fhicl);
    } else {
      throw artdaq::database::invalid_option_exception(apifunctname)
          << "Unsupported storage format " << demangle(typeid(TYPE).name()) << ",  use either JsonData or FhiclData.";
    }

    if (!entity.empty()) opts.entity(entity);

    auto buffer = std::string{};

    auto apiCallResult = impl::read_document(opts, buffer);

    if (!apiCallResult.first) throw artdaq::database::runtime_exception(apifunctname) << apiCallResult.second;

    auto data = TYPE{buffer};

    auto readResult = serializer.template readDocument<TYPE>(data);

    if (!readResult.first) throw artdaq::database::runtime_exception(apifunctname) << readResult.second;

    return readResult;
  } catch (std::exception const& e) {
    return {false, {e.what()}};
  }

  //==============================================================================
  // returns a list of configuration versions for user specified entity and
  // configuration name
  template <typename CONF>
  std::list<std::string> getVersions(CONF configuration, std::string const& entity) const {
    auto returnList = std::list<std::string>();  // RVO

    constexpr auto apifunctname = "ConfigurationInterface::getVersions";

    try {
      auto serializer = ConfigurationSerializer<CONF, MakeSerializable>::wrap(configuration);

      auto opts = ManageDocumentOperation{apiname};

      opts.operation(apiliteral::operation::findversions);
      opts.collection(serializer.configurationName());
      opts.format(data_format_t::gui);

      if (!entity.empty()) opts.entity(entity);

      auto apiCallResult = impl::find_versions(opts);

      if (!apiCallResult.first) throw artdaq::database::runtime_exception(apifunctname) << apiCallResult.second;

      auto resultAST = jsn::object_t{};

      if (!jsn::JsonReader().read(apiCallResult.second, resultAST))
        throw artdaq::database::runtime_exception(apifunctname) << "Invalid JSON:" << apiCallResult.second;
      try {
        auto const& searches = boost::get<jsn::array_t>(resultAST.at(jsonliteral::search));

        for (auto const search : searches) {
          auto const& query = boost::get<jsn::object_t>(search).at(jsonliteral::query);
          auto const& filter = boost::get<jsn::object_t>(query).at(jsonliteral::filter);
          auto const& version =
              boost::get<std::string>(boost::get<jsn::object_t>(filter).at(apiliteral::filter::version));
          returnList.emplace_back(version);
        }
      } catch (std::exception const& e) {
        throw artdaq::database::runtime_exception(apifunctname)
            << "Unable to read versions from  JSON:" << apiCallResult.second << "; Exception:" << e.what();
      }
    } catch (std::exception const& e) {
      returnList.clear();
      throw artdaq::database::runtime_exception(apifunctname) << "Exception:" << e.what();
    } catch (...) {
      returnList.clear();
      throw artdaq::database::runtime_exception(apifunctname) << "Unknown exception";
    }

    return returnList;  // RVO
  }

  //==============================================================================
  // returns a set of global configuration names matching the mongosearch
  // criteria, or all global configuration names if criteria is emapty.
  std::set<std::string> findGlobalConfigurations(std::string const& mongosearch) const {
    auto returnSet = std::set<std::string>{};  // RVO

    constexpr auto apifunctname = "ConfigurationInterface::findGlobalConfigurations";

    try {
      auto opts = ManageDocumentOperation{apiname};
      opts.operation(apiliteral::operation::findconfigs);
      opts.format(data_format_t::gui);

      if (!mongosearch.empty()) {
        if (opts.provider() == apiliteral::provider::mongo && mongosearch != "*") {
          opts.configuration(mongosearch);
        } else if (opts.provider() == apiliteral::provider::filesystem) {
          opts.configuration(mongosearch);
        }
      }

      auto apiCallResult = impl::find_configurations(opts);

      if (!apiCallResult.first) throw artdaq::database::runtime_exception(apifunctname) << apiCallResult.second;

      auto resultAST = jsn::object_t{};

      if (!jsn::JsonReader().read(apiCallResult.second, resultAST))
        throw artdaq::database::runtime_exception(apifunctname) << "Invalid JSON:" << apiCallResult.second;

      try {
        auto const& searches = boost::get<jsn::array_t>(resultAST.at(jsonliteral::search));

        for (auto const search : searches) {
          auto const& query = boost::get<jsn::object_t>(search).at(jsonliteral::query);
          auto const& filter = boost::get<jsn::object_t>(query).at(jsonliteral::filter);
          auto const& configuration =
              boost::get<std::string>(boost::get<jsn::object_t>(filter).at(apiliteral::filter::configurations));
          returnSet.insert(configuration);
        }
      } catch (std::exception const& e) {
        throw artdaq::database::runtime_exception(apifunctname)
            << "Unable to read configuration names from  JSON:" << apiCallResult.second << "; Exception:" << e.what();
      }
    } catch (std::exception const& e) {
      returnSet.clear();
      throw artdaq::database::runtime_exception(apifunctname) << "Exception:" << e.what();
    } catch (...) {
      returnSet.clear();
      throw artdaq::database::runtime_exception(apifunctname) << "Unknown exception";
    }

    return returnSet;  // RVO
  }

  //==============================================================================
  //
  VersionInfoList_t loadGlobalConfiguration(std::string const& configuration) const {
    confirm(!configuration.empty());

    constexpr auto apifunctname = "ConfigurationInterface::loadGlobalConfiguration";

    auto returnList = VersionInfoList_t{};  // RVO

    auto to_VersionInfo = [](auto const& query) {
      auto const& configuration =
          boost::get<std::string>(boost::get<jsn::object_t>(query).at(apiliteral::option::collection));
      auto const& filter = boost::get<jsn::object_t>(query).at(jsonliteral::filter);
      auto const& version = boost::get<std::string>(boost::get<jsn::object_t>(filter).at(apiliteral::filter::version));
      auto const& entity = boost::get<std::string>(boost::get<jsn::object_t>(filter).at(apiliteral::filter::entities));
      return VersionInfo{configuration, version, entity};
    };

    try {
      auto opts = ManageDocumentOperation{apiname};
      opts.operation(apiliteral::operation::confcomposition);
      opts.format(data_format_t::gui);

      if (!configuration.empty()) opts.configuration(configuration);

      auto apiCallResult = impl::configuration_composition(opts);

      if (!apiCallResult.first) throw artdaq::database::runtime_exception(apifunctname) << apiCallResult.second;

      auto resultAST = jsn::object_t{};

      if (!jsn::JsonReader().read(apiCallResult.second, resultAST))
        throw artdaq::database::runtime_exception(apifunctname) << "Invalid JSON:" << apiCallResult.second;

      try {
        for (auto const search : boost::get<jsn::array_t>(resultAST.at(jsonliteral::search))) {
          auto buffer = std::string{};

          {
            auto query = boost::get<jsn::object_t>(search).at(jsonliteral::query);

            boost::get<jsn::object_t>(query).at("operation") = std::string{apiliteral::operation::findversions};

            if (!jsn::JsonWriter().write(boost::get<jsn::object_t>(query), buffer))
              throw artdaq::database::runtime_exception(apifunctname) << "Invalid JSON:" << apiCallResult.second;
          }

          auto apiCall2Result = cf::json::find_versions(buffer);

          if (!apiCall2Result.first) throw artdaq::database::runtime_exception(apifunctname) << apiCall2Result.second;

          auto result2AST = jsn::object_t{};

          if (!jsn::JsonReader().read(apiCall2Result.second, result2AST))
            throw artdaq::database::runtime_exception(apifunctname) << "Invalid JSON:" << apiCall2Result.second;

          auto const& searches2 = boost::get<jsn::array_t>(result2AST.at(jsonliteral::search));

          if (searches2.size() != 1)
            throw artdaq::database::runtime_exception(apifunctname) << "Invalid JSON:" << apiCall2Result.second
                                                                    << "Too many results found, expected one.";

          auto const& version_query = boost::get<jsn::object_t>(*searches2.begin()).at(jsonliteral::query);

          returnList.emplace_back(to_VersionInfo(version_query));
        }
      } catch (std::exception const& e) {
        throw artdaq::database::runtime_exception(apifunctname)
            << "Unable to read version info names from  JSON:" << apiCallResult.second << "; Exception:" << e.what();
      }

    } catch (std::exception const& e) {
      returnList.clear();
      throw artdaq::database::runtime_exception(apifunctname) << "Exception:" << e.what();

    } catch (...) {
      returnList.clear();
      throw artdaq::database::runtime_exception(apifunctname) << "Unknown exception";
    }

    return returnList;  // RVO
  }

  //==============================================================================
  //
  cf::result_t storeGlobalConfiguration(VersionInfoList_t const& versionInfoList,
                                        std::string const& configuration) const try {
    confirm(!configuration.empty());
    confirm(!versionInfoList.empty());

    constexpr auto apifunctname = "ConfigurationInterface::storeGlobalConfiguration";

    if (versionInfoList.empty())
      throw artdaq::database::invalid_option_exception(apifunctname) << "Version info list is empty";

    if (configuration.empty())
      throw artdaq::database::invalid_option_exception(apifunctname) << "Global configuration name is empty";

    auto payloadAST = jsn::object_t{};
    payloadAST[apiliteral::operations] = jsn::array_t{};
    auto& operations = boost::get<jsn::array_t>(payloadAST[apiliteral::operations]);

    for (auto const& versionInfo : versionInfoList) {
      versionInfo.validate();

      auto op = jsn::object_t{};
      op[apiliteral::option::configuration] = configuration;
      op[apiliteral::option::collection] = versionInfo.configuration;
      op[apiliteral::option::format] = to_string(data_format_t::gui);
      op[apiliteral::option::operation] = std::string{apiliteral::operation::assignconfig};
      op[jsonliteral::filter] = jsn::object_t{};

      auto& filter = boost::get<jsn::object_t>(op[jsonliteral::filter]);

      filter[apiliteral::filter::version] = versionInfo.version;
      filter[apiliteral::filter::entities] = versionInfo.entity;

      operations.push_back(op);
    }

    auto buffer = std::string{};

    if (!jsn::JsonWriter().write(payloadAST, buffer))
      throw artdaq::database::runtime_exception(apifunctname) << "JsonWriter failed, invalid payloadAST.";

    auto apiCallResult = cf::json::create_configuration(buffer);

    if (!apiCallResult.first) throw artdaq::database::runtime_exception(apifunctname) << apiCallResult.second;

    return apiCallResult;
  } catch (std::exception const& e) {
    return {false, make_error_msg(e.what())};
  } catch (...) {
    return {false, make_error_msg("Unknown exception")};
  }

  // returns a set of configuration collection names
  std::set<std::string> listCollections(std::string const& name_prefix) const {
    auto returnSet = std::set<std::string>{};  // RVO

    constexpr auto apifunctname = "ConfigurationInterface::listCollections";

    auto opts = ManageDocumentOperation{apiname};

    opts.operation(apiliteral::operation::listcollections);
    opts.format(data_format_t::gui);

    if (!name_prefix.empty()) opts.collection(name_prefix);

    auto apiCallResult = cf::impl::list_collections(opts);

    if (!apiCallResult.first) throw artdaq::database::runtime_exception(apifunctname) << apiCallResult.second;

    auto payloadAST = jsn::object_t{};

    if (!jsn::JsonReader().read(apiCallResult.second, payloadAST))
      throw artdaq::database::runtime_exception(apifunctname) << "JsonWriter failed, invalid payloadAST.";

    jsn::array_t const& searchResults = boost::get<jsn::array_t>(payloadAST.at(jsonliteral::search));

    for (auto const& searchResult : searchResults) {
      auto const& query = boost::get<jsn::object_t>(searchResult).at(jsonliteral::query);
      auto const& collection = boost::get<jsn::object_t>(query).at(apiliteral::option::collection);
      returnSet.insert(boost::get<std::string>(collection));
    }

    return returnSet;
  }

  // defaults
  ~ConfigurationInterface() = default;
  ConfigurationInterface(ConfigurationInterface&&) = default;
  ConfigurationInterface() = default;

  // deleted
  ConfigurationInterface(ConfigurationInterface const&) = delete;
  ConfigurationInterface& operator=(ConfigurationInterface const&) = delete;
  ConfigurationInterface& operator=(ConfigurationInterface&&) = delete;
};

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDBIFC_H_ */
