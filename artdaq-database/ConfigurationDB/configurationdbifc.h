#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDBIFC_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDBIFC_H_

#include <map>
#include <set>

#include "artdaq-database/ConfigurationDB/Multitasker.h"
#include "artdaq-database/ConfigurationDB/configurationdbifc_base.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "dboperation_findcompositions.h"
#include "options_operation_manageconfigs.h"

using debug::demangle;

namespace artdaq {
namespace database {
namespace configuration {

namespace db = artdaq::database;
namespace jsonliteral = db::dataformats::literal;

namespace cf = db::configuration;
namespace impl = cf::opts;

using artdaq::database::basictypes::FhiclData;
using artdaq::database::basictypes::JsonData;
using artdaq::database::sharedtypes::unwrap;
using cf::options::data_format_t;

constexpr auto apiname = "ConfigurationInterface";

struct ConfigurationInterface final {
  using VersionInfoList_t = std::list<VersionInfo>;

  ConfigurationInterface(std::string const&) {};

  //==============================================================================
  // stores configuration version to database
  template <typename CONF, typename TYPE>
  cf::result_t storeVersion(CONF configuration, std::string const& version, std::string const& entity) const noexcept try {
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
  // overwrite configuration version to database
  template <typename CONF, typename TYPE>
  cf::result_t overwriteVersion(CONF configuration, std::string const& version, std::string const& entity) const noexcept try {
    confirm(!version.empty());

    constexpr auto apifunctname = "ConfigurationInterface::updateVersion";

    if (version.empty()) throw artdaq::database::invalid_option_exception(apifunctname) << "Version is empty";

    auto serializer = ConfigurationSerializer<CONF, MakeSerializable>::wrap(configuration);

    auto opts = ManageDocumentOperation{apiname};

    opts.collection(serializer.configurationName());
    opts.version(version);

    if (!entity.empty()) opts.entity(entity);

    auto buffer = std::string{};

    opts.format(data_format_t::db);
    opts.operation(apiliteral::operation::readdocument);

    auto apiCallResult = impl::read_document(opts, buffer);

    if (!apiCallResult.first) throw artdaq::database::runtime_exception(apifunctname) << apiCallResult.second;

    if (std::is_same<TYPE, JsonData>::value) {
      opts.format(data_format_t::json);
    } else if (std::is_same<TYPE, FhiclData>::value) {
      opts.format(data_format_t::fhicl);
    } else {
      throw artdaq::database::invalid_option_exception(apifunctname)
          << "Unsupported storage format " << demangle(typeid(TYPE).name()) << ",  use either JsonData or FhiclData.";
    }

    auto data = TYPE{"{}"};

    auto writeResult = serializer.template writeDocument<TYPE>(data);

    if (!writeResult.first) throw artdaq::database::runtime_exception(apifunctname) << writeResult.second;

    auto wraped_data = JsonData{std::string("{\"data\":").append(data).append("}")};

    std::ostringstream oss;
    oss << db::docrecord::JSONDocumentBuilder().createFromData(wraped_data);

    auto newRecord = jsn::object_t{};

    if (!db::json::JsonReader{}.read(oss.str(), newRecord))
      throw artdaq::database::runtime_exception(apifunctname) << "Not a JSON object" << oss.str();

    auto dbRecord = jsn::object_t{};

    if (!db::json::JsonReader{}.read(buffer, dbRecord)) throw artdaq::database::runtime_exception(apifunctname) << "Not a JSON object" << buffer;

    dbRecord[jsonliteral::document].swap(newRecord[jsonliteral::document]);

    buffer.clear();

    if (!db::json::JsonWriter{}.write(dbRecord, buffer)) throw artdaq::database::runtime_exception(apifunctname) << "Not a JSON object" << buffer;

    opts.operation(apiliteral::operation::overwritedocument);
    opts.format(data_format_t::db);

    apiCallResult = impl::write_document(opts, buffer);

    if (!apiCallResult.first) throw artdaq::database::runtime_exception(apifunctname) << apiCallResult.second;

    return apiCallResult;
  } catch (std::exception const& e) {
    return {false, {e.what()}};
  }

  //==============================================================================
  // marks configuration version as read-only in database
  template <typename CONF, typename TYPE>
  cf::result_t markVersionReadonly(CONF configuration, std::string const& version, std::string const& entity) const noexcept try {
    confirm(!version.empty());

    constexpr auto apifunctname = "ConfigurationInterface::markVersionReadonly";

    if (version.empty()) throw artdaq::database::invalid_option_exception(apifunctname) << "Version is empty";

    auto serializer = ConfigurationSerializer<CONF, MakeSerializable>::wrap(configuration);

    auto opts = ManageDocumentOperation{apiname};

    opts.operation(apiliteral::operation::markreadonly);
    opts.collection(serializer.configurationName());
    opts.version(version);

    if (!entity.empty()) opts.entity(entity);

    auto apiCallResult = impl::mark_document_readonly(opts);

    if (!apiCallResult.first) throw artdaq::database::runtime_exception(apifunctname) << apiCallResult.second;

    return apiCallResult;
  } catch (std::exception const& e) {
    return {false, {e.what()}};
  }

  //==============================================================================
  // loads configuration version from database
  template <typename CONF, typename TYPE>
  cf::result_t loadVersion(CONF configuration, std::string const& version, std::string const& entity) const noexcept try {
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
        auto const& searches = unwrap(resultAST).value_as<const jsn::array_t>(jsonliteral::search);

        for (auto const& search : searches) {
          auto const& query = unwrap(search).value_as<const jsn::object_t>(jsonliteral::query);
          auto const& filter = unwrap(query).value_as<const jsn::object_t>(jsonliteral::filter);
          auto const& version = unwrap(filter).value_as<const std::string>(apiliteral::filter::version);
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
  // returns a set of global configuration names matching the search
  // criteria, or all global configuration names if criteria is emapty.
  std::set<std::string> findGlobalConfigurations(std::string const& search) const {
    auto returnSet = std::set<std::string>{};  // RVO

    constexpr auto apifunctname = "ConfigurationInterface::findGlobalConfigurations";

    try {
      auto opts = ManageDocumentOperation{apiname};
      opts.operation(apiliteral::operation::findconfigs);
      opts.format(data_format_t::gui);
      opts.configuration("*");

      if (!search.empty()) opts.configuration(search);

      auto apiCallResult = impl::find_configurations(opts);

      if (!apiCallResult.first) throw artdaq::database::runtime_exception(apifunctname) << apiCallResult.second;

      auto resultAST = jsn::object_t{};

      if (!jsn::JsonReader().read(apiCallResult.second, resultAST))
        throw artdaq::database::runtime_exception(apifunctname) << "Invalid JSON:" << apiCallResult.second;

      try {
        auto const& searches = unwrap(resultAST).value_as<jsn::array_t>(jsonliteral::search);

        for (auto const& search : searches) {
          auto const& query = unwrap(search).value_as<const jsn::object_t>(jsonliteral::query);
          auto const& filter = unwrap(query).value_as<const jsn::object_t>(jsonliteral::filter);
          auto const& configuration = unwrap(filter).value_as<const std::string>(apiliteral::filter::configurations);
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
  }  // namespace configuration

  //==============================================================================
  //
  VersionInfoList_t loadGlobalConfiguration(std::string const& configuration) const {
    confirm(!configuration.empty());

    constexpr auto apifunctname = "ConfigurationInterface::loadGlobalConfiguration";

    auto returnList = VersionInfoList_t{};  // RVO

    auto to_VersionInfo = [](jsn::object_t const& query) {
      auto const& configuration = unwrap(query).value_as<const std::string>(apiliteral::option::collection);
      auto const& filter = unwrap(query).value_as<const jsn::object_t>(jsonliteral::filter);
      auto const& version = unwrap(filter).value_as<const std::string>(apiliteral::filter::version);
      auto const& entity = unwrap(filter).value_as<const std::string>(apiliteral::filter::entities);
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
        auto& search_array = unwrap(resultAST).value_as<jsn::array_t>(jsonliteral::search);
        for (auto search : search_array) {
          auto buffer = std::string{};

          {
            auto& query = unwrap(search).value_as<jsn::object_t>(jsonliteral::query);

            query[apiliteral::option::operation] = std::string{apiliteral::operation::findversions};

            if (!jsn::JsonWriter().write(query, buffer))
              throw artdaq::database::runtime_exception(apifunctname) << "Invalid JSON:" << apiCallResult.second;
          }

          auto apiCall2Result = cf::json::find_versions(buffer);

          if (!apiCall2Result.first) throw artdaq::database::runtime_exception(apifunctname) << apiCall2Result.second;

          auto result2AST = jsn::object_t{};

          if (!jsn::JsonReader().read(apiCall2Result.second, result2AST))
            throw artdaq::database::runtime_exception(apifunctname) << "Invalid JSON:" << apiCall2Result.second;

          auto const& searches2 = unwrap(result2AST).value_as<jsn::array_t>(jsonliteral::search);

          if (searches2.size() != 1)
            throw artdaq::database::runtime_exception(apifunctname)
                << "Invalid JSON:" << apiCall2Result.second << "Too many results found, expected one.";

          auto const& version_query = unwrap(*searches2.begin()).value_as<const jsn::object_t>(jsonliteral::query);

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
  // storeGlobalConfiguration - stores a composition (global configuration)
  // By default, rejects duplicate composition names. Use allowOverwrite=true to update existing.
  // When allowOverwrite=true, performs a merge-based overwrite:
  //   - Unassigns old members that are no longer in the composition
  //   - Assigns only new members (skips unchanged ones)
  cf::result_t storeGlobalConfiguration(VersionInfoList_t const& versionInfoList, std::string const& configuration, bool allowOverwrite = false) const
      try {
    constexpr auto apifunctname = "ConfigurationInterface::storeGlobalConfiguration";

    if (versionInfoList.empty()) {
      throw artdaq::database::invalid_option_exception(apifunctname) << "Version info list is empty. Compositions must have at least one member.";
    }

    if (configuration.empty()) {
      throw artdaq::database::invalid_option_exception(apifunctname) << "Global configuration name is empty";
    }

    bool composition_exists = false;
    try {
      auto existingConfigs = findGlobalConfigurations(configuration);
      composition_exists = (existingConfigs.find(configuration) != existingConfigs.end());

      if (composition_exists && !allowOverwrite) {
        throw artdaq::database::runtime_exception(apifunctname) << "Global configuration '" << configuration << "' already exists. "
                                                                << "Use allowOverwrite=true to update an existing configuration.";
      }
    } catch (artdaq::database::runtime_exception const&) {
      throw;
    } catch (...) {
    }

    VersionInfoList_t old_members;
    if (composition_exists && allowOverwrite) {
      try {
        old_members = loadGlobalConfiguration(configuration);
        TLOG(TLVL_DEBUG) << apifunctname << ": Loaded " << old_members.size() << " existing members for merge";
      } catch (std::exception const& e) {
        TLOG(TLVL_WARNING) << apifunctname << ": Could not load existing composition for merge: " << e.what();
        old_members.clear();
      }
    }

    using MembershipKey = std::pair<std::string, std::string>;
    auto make_key = [](VersionInfo const& vi) -> MembershipKey { return {vi.configuration, vi.entity}; };

    std::map<MembershipKey, std::string> new_map;  // key → version
    for (auto const& new_member : versionInfoList) {
      new_map[make_key(new_member)] = new_member.version;
    }

    VersionInfoList_t to_unassign;
    std::set<MembershipKey> unchanged;

    for (auto const& old_member : old_members) {
      auto key = make_key(old_member);
      auto it = new_map.find(key);

      if (it == new_map.end()) {
        to_unassign.push_back(old_member);
        TLOG(TLVL_DEBUG) << apifunctname << ": Will unassign (removed): " << old_member.configuration << "/" << old_member.version << "/"
                         << old_member.entity;
      } else if (it->second != old_member.version) {
        to_unassign.push_back(old_member);
        TLOG(TLVL_DEBUG) << apifunctname << ": Will unassign (replaced): " << old_member.configuration << "/" << old_member.version << "/"
                         << old_member.entity;
      } else {
        unchanged.insert(key);
        TLOG(TLVL_DEBUG) << apifunctname << ": Unchanged: " << old_member.configuration << "/" << old_member.version << "/" << old_member.entity;
      }
    }

    VersionInfoList_t to_assign;
    for (auto const& new_member : versionInfoList) {
      if (unchanged.find(make_key(new_member)) == unchanged.end()) {
        to_assign.push_back(new_member);
        TLOG(TLVL_DEBUG) << apifunctname << ": Will assign: " << new_member.configuration << "/" << new_member.version << "/" << new_member.entity;
      }
    }

    TLOG(TLVL_DEBUG) << apifunctname << ": Executing " << to_unassign.size() << " unassignments";
    for (auto const& old_member : to_unassign) {
      try {
        auto removeOp = jsn::object_t{};
        removeOp[apiliteral::option::configuration] = configuration;
        removeOp[apiliteral::option::collection] = old_member.configuration;
        removeOp[apiliteral::option::format] = to_string(data_format_t::gui);
        removeOp[apiliteral::option::operation] = std::string{apiliteral::operation::removeconfig};
        removeOp[apiliteral::option::searchfilter] = jsn::object_t{};

        auto& filter = unwrap(removeOp).value_as<jsn::object_t>(apiliteral::option::searchfilter);
        filter[apiliteral::filter::version] = old_member.version;
        filter[apiliteral::filter::entities] = old_member.entity;

        auto buffer = std::string{};
        if (jsn::JsonWriter().write(removeOp, buffer)) {
          TLOG(TLVL_DEBUG) << apifunctname << ": Calling remove_configuration with: " << buffer;
          auto result = cf::json::remove_configuration(buffer);
          if (!result.first) {
            TLOG(TLVL_WARNING) << apifunctname << ": Failed to unassign " << old_member.configuration << "/" << old_member.version << ": "
                               << result.second;
          } else {
            TLOG(TLVL_DEBUG) << apifunctname << ": Successfully unassigned " << old_member.configuration << "/" << old_member.version;
          }
        }
      } catch (std::exception const& e) {
        TLOG(TLVL_WARNING) << apifunctname << ": Exception unassigning " << old_member.configuration << "/" << old_member.version << ": " << e.what();
      }
    }

    VersionInfoList_t const& members_to_process = (composition_exists && allowOverwrite) ? to_assign : versionInfoList;

    if (members_to_process.empty()) {
      TLOG(TLVL_DEBUG) << apifunctname << ": No changes needed, all members unchanged";
      return {true, "Success"};
    }

    auto payloadAST = jsn::object_t{};
    payloadAST[apiliteral::operations] = jsn::array_t{};
    auto& operations = unwrap(payloadAST).value_as<jsn::array_t>(apiliteral::operations);

    for (auto const& versionInfo : members_to_process) {
      versionInfo.validate();

      auto op = jsn::object_t{};
      op[apiliteral::option::configuration] = configuration;
      op[apiliteral::option::collection] = versionInfo.configuration;
      op[apiliteral::option::format] = to_string(data_format_t::gui);
      op[apiliteral::option::operation] = std::string{apiliteral::operation::assignconfig};
      op[jsonliteral::filter] = jsn::object_t{};

      auto& filter = unwrap(op).value_as<jsn::object_t>(jsonliteral::filter);

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

  //==============================================================================
  // storeGlobalConfiguration_mt - multi-threaded version of storeGlobalConfiguration
  //
  cf::result_t storeGlobalConfiguration_mt(VersionInfoList_t const& versionInfoList, std::string const& configuration, bool overwrite = false) const
      try {
    constexpr auto apifunctname = "ConfigurationInterface::storeGlobalConfiguration_mt";

    if (versionInfoList.empty()) {
      throw artdaq::database::invalid_option_exception(apifunctname) << "Version info list is empty. Compositions must have at least one member.";
    }

    if (configuration.empty()) {
      throw artdaq::database::invalid_option_exception(apifunctname) << "Global configuration name is empty";
    }

    bool composition_exists = false;
    VersionInfoList_t old_members;
    try {
      auto existingConfigs = findGlobalConfigurations(configuration);
      composition_exists = (existingConfigs.find(configuration) != existingConfigs.end());
      if (composition_exists) {
        old_members = loadGlobalConfiguration(configuration);
        TLOG(TLVL_DEBUG) << apifunctname << ": Composition exists with " << old_members.size() << " members";
      }
    } catch (...) {
    }

    if (composition_exists && !overwrite) {
      throw artdaq::database::runtime_exception(apifunctname) << "Global configuration '" << configuration << "' already exists. "
                                                              << "Use allowOverwrite=true to update an existing configuration.";
    }

    VersionInfoList_t members_to_assign;

    if (composition_exists && overwrite) {
      using MembershipKey = std::pair<std::string, std::string>;
      auto make_key = [](VersionInfo const& vi) -> MembershipKey { return {vi.configuration, vi.entity}; };

      std::map<MembershipKey, std::string> new_map;
      for (auto const& new_member : versionInfoList) {
        new_map[make_key(new_member)] = new_member.version;
      }

      VersionInfoList_t to_unassign;
      std::set<MembershipKey> unchanged;

      for (auto const& old_member : old_members) {
        auto key = make_key(old_member);
        auto it = new_map.find(key);

        if (it == new_map.end()) {
          to_unassign.push_back(old_member);
          TLOG(TLVL_DEBUG) << apifunctname << ": Will unassign (removed): " << old_member.configuration << "/" << old_member.version;
        } else if (it->second != old_member.version) {
          to_unassign.push_back(old_member);
          TLOG(TLVL_DEBUG) << apifunctname << ": Will unassign (replaced): " << old_member.configuration << "/" << old_member.version;
        } else {
          unchanged.insert(key);
          TLOG(TLVL_DEBUG) << apifunctname << ": Unchanged: " << old_member.configuration << "/" << old_member.version;
        }
      }

      TLOG(TLVL_DEBUG) << apifunctname << ": Executing " << to_unassign.size() << " unassignments";
      for (auto const& old_member : to_unassign) {
        try {
          auto removeOp = jsn::object_t{};
          removeOp[apiliteral::option::configuration] = configuration;
          removeOp[apiliteral::option::collection] = old_member.configuration;
          removeOp[apiliteral::option::format] = to_string(data_format_t::gui);
          removeOp[apiliteral::option::operation] = std::string{apiliteral::operation::removeconfig};
          removeOp[apiliteral::option::searchfilter] = jsn::object_t{};

          auto& filter = unwrap(removeOp).value_as<jsn::object_t>(apiliteral::option::searchfilter);
          filter[apiliteral::filter::version] = old_member.version;
          filter[apiliteral::filter::entities] = old_member.entity;

          auto buffer = std::string{};
          if (jsn::JsonWriter().write(removeOp, buffer)) {
            auto result = cf::json::remove_configuration(buffer);
            if (!result.first) {
              TLOG(TLVL_WARNING) << apifunctname << ": Failed to unassign " << old_member.configuration << "/" << old_member.version << ": "
                                 << result.second;
            }
          }
        } catch (std::exception const& e) {
          TLOG(TLVL_WARNING) << apifunctname << ": Exception unassigning " << old_member.configuration << "/" << old_member.version << ": "
                             << e.what();
        }
      }

      for (auto const& new_member : versionInfoList) {
        if (unchanged.find(make_key(new_member)) == unchanged.end()) {
          members_to_assign.push_back(new_member);
        }
      }
    } else {
      members_to_assign = versionInfoList;
    }

    if (members_to_assign.empty()) {
      TLOG(TLVL_DEBUG) << apifunctname << ": No changes needed, all members unchanged";
      return {true, "Success"};
    }

    TLOG(TLVL_DEBUG) << apifunctname << ": Assigning " << members_to_assign.size() << " members in parallel";

    auto threadCount = std::min<std::size_t>(members_to_assign.size(), std::thread::hardware_concurrency() / 2);
    Multitasker multitasker(threadCount);

    for (const auto& versionInfo : members_to_assign) {
      multitasker.addTask([configuration, versionInfo] {
        auto op = jsn::object_t{};
        op[apiliteral::option::configuration] = configuration;
        op[apiliteral::option::collection] = versionInfo.configuration;
        op[apiliteral::option::format] = to_string(data_format_t::gui);
        op[apiliteral::option::operation] = std::string{apiliteral::operation::assignconfig};
        op[jsonliteral::filter] = jsn::object_t{};

        auto& filter = unwrap(op).value_as<jsn::object_t>(jsonliteral::filter);
        filter[apiliteral::filter::version] = versionInfo.version;
        filter[apiliteral::filter::entities] = versionInfo.entity;

        auto buffer = std::string{};
        if (!jsn::JsonWriter().write(op, buffer)) {
          return cf::result_t{false, "JsonWriter failed for assign operation"};
        }

        return cf::json::assign_configuration(buffer);
      });
    }

    multitasker.waitForResults();
    auto apiCallResult = multitasker.getMergedResults();

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

    jsn::array_t const& searchResults = unwrap(payloadAST).value_as<jsn::array_t>(jsonliteral::search);

    for (auto const& searchResult : searchResults) {
      auto const& query = unwrap(searchResult).value_as<const jsn::object_t>(jsonliteral::query);
      auto const& collection = unwrap(query).value_as<const std::string>(apiliteral::option::collection);
      returnSet.insert(collection);
    }

    return returnSet;
  }

  std::set<std::string> findGlobalConfigurationsContaining(std::string const& configurationType, std::string const& version,
                                                           std::string const& entity = "") const {
    constexpr auto apifunctname = "ConfigurationInterface::findGlobalConfigurationsContaining";

    auto returnSet = std::set<std::string>{};  // RVO

    try {
      if (configurationType.empty()) {
        throw artdaq::database::invalid_option_exception(apifunctname) << "Configuration type is empty";
      }

      if (version.empty()) {
        throw artdaq::database::invalid_option_exception(apifunctname) << "Version is empty";
      }

      try {
        auto opts = ManageDocumentOperation{apiname};
        opts.operation(apiliteral::operation::findcompositionscontaining);
        opts.collection(configurationType);
        opts.version(version);
        if (!entity.empty()) opts.entity(entity);
        opts.format(data_format_t::gui);

        std::string optimizedResults;
        auto apiResult = impl::find_compositions_containing(opts, optimizedResults);

        if (apiResult.first && !optimizedResults.empty()) {
          auto resultAST = jsn::object_t{};
          if (jsn::JsonReader().read(optimizedResults, resultAST)) {
            try {
              auto const& searches = unwrap(resultAST).value_as<jsn::array_t>(jsonliteral::search);
              if (!searches.empty()) {
                for (auto const& search : searches) {
                  auto const& name_value = unwrap(search).value_as<const jsn::object_t>(jsonliteral::name);
                  if (!name_value.empty()) {
                    auto const& compositionName = unwrap(search).value_as<const std::string>(apiliteral::name);
                    returnSet.insert(compositionName);
                  }
                }
                return returnSet;
              }
            } catch (...) {
            }
          }
        }
      } catch (...) {
      }

      auto allCompositions = findGlobalConfigurations("");

      for (auto const& compositionName : allCompositions) {
        try {
          auto members = loadGlobalConfiguration(compositionName);

          for (auto const& member : members) {
            bool typeMatches = (member.configuration == configurationType);
            bool versionMatches = (member.version == version);
            bool entityMatches = (entity.empty() || member.entity == entity);

            if (typeMatches && versionMatches && entityMatches) {
              returnSet.insert(compositionName);
              break;
            }
          }
        } catch (std::exception const& e) {
          continue;
        }
      }
    } catch (std::exception const& e) {
      returnSet.clear();
      throw artdaq::database::runtime_exception(apifunctname) << "Exception: " << e.what();
    } catch (...) {
      returnSet.clear();
      throw artdaq::database::runtime_exception(apifunctname) << "Unknown exception";
    }

    return returnSet;  // RVO
  }

  // defaults
  ~ConfigurationInterface() = default;
  ConfigurationInterface(ConfigurationInterface&&) = default;
  ConfigurationInterface() = default;

  // deleted
  ConfigurationInterface(ConfigurationInterface const&) = delete;
  ConfigurationInterface& operator=(ConfigurationInterface const&) = delete;
  ConfigurationInterface& operator=(ConfigurationInterface&&) = delete;
};  // namespace configuration

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDBIFC_H_ */
