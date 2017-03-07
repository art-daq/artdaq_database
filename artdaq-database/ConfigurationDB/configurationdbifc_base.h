#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDBIFC_BASE_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDBIFC_BASE_H_

//#include <memory>
//#include <string>
//#include <type_traits>
#include <utility>

#include "artdaq-database/BasicTypes/basictypes.h"

#include "artdaq-database/ConfigurationDB/configurationdb.h"
#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/Json/json_writer.h"

#include "artdaq-database/ConfigurationDB/options_operation_managedocument.h"

namespace debug {
std::string demangle(const char*);
}

using debug::demangle;

namespace artdaq {
namespace database {
namespace configuration {

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace impl = cf::opts;

using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::FhiclData;
using cf::options::data_format_t;

auto make_error_msg = [](const char* msg) { return std::string("{error:\"").append(msg).append("\"}"); };

template <typename CONF>
class MakeSerializable final {
  static_assert(std::is_pointer<CONF>::value || std::is_reference<CONF>::value,
                "Template parameter must be either a pointer or reference type");

 public:
  template <class TYPE>
  bool writeDocument(TYPE& data) const throw(artdaq::database::exception) try {
    return writeDocumentImpl(data);
  } catch (std::exception const& e) {
    throw artdaq::database::exception("MakeSerializable::writeDocument")
        << "Unable to write " << demangle(typeid(CONF).name()) << " into " << demangle(typeid(TYPE).name())
        << "; Exception msg:" << e.what() << ".";
  } catch (...) {
    throw artdaq::database::exception("MakeSerializable::writeDocument")
        << "Unable to write " << demangle(typeid(CONF).name()) << " into " << demangle(typeid(TYPE).name()) << ".";
  }

  template <class TYPE>
  bool readDocument(TYPE const& data) throw(artdaq::database::exception) try {
    return readDocumentImpl(data);
  } catch (std::exception const& e) {
    throw artdaq::database::exception("MakeSerializable::readDocument")
        << "Unable to read " << demangle(typeid(CONF).name()) << " from " << demangle(typeid(TYPE).name())
        << "; Exception msg:" << e.what() << ".";
  } catch (...) {
    throw artdaq::database::exception("MakeSerializable::readDocument")
        << "Unable to read " << demangle(typeid(CONF).name()) << " from " << demangle(typeid(TYPE).name()) << ".";
  }

  std::string configurationName() const throw(artdaq::database::exception) try {
    return configurationNameImpl();
  } catch (std::exception const& e) {
    throw artdaq::database::exception("MakeSerializable::configurationName")
        << "Unable to get configuration name" << demangle(typeid(CONF).name()) << "; Exception msg:" << e.what() << ".";
  } catch (...) {
    throw artdaq::database::exception("MakeSerializable::configurationName") << "Unable to to get configuration name"
                                                                             << demangle(typeid(CONF).name()) << ".";
  }

  MakeSerializable(CONF conf) : _conf(conf){};

  // defaults
  ~MakeSerializable() = default;

  // deleted
  MakeSerializable() = delete;
  MakeSerializable(MakeSerializable&&) = delete;
  MakeSerializable(MakeSerializable const&) = delete;
  MakeSerializable& operator=(MakeSerializable const&) = delete;
  MakeSerializable& operator=(MakeSerializable&&) = delete;

 private:
  // readImpl and writeImpl methods should be specialized
  // for concrete configuration types
  template <class TYPE>
  bool writeDocumentImpl(TYPE&) const;

  template <class TYPE>
  bool readDocumentImpl(TYPE const&);

  std::string configurationNameImpl() const;

 private:
  CONF _conf;
};

template <typename CONF, template <typename ARG = CONF> class SERIALIZABLE>
class ConfigurationSerializer final {
  static_assert(std::is_pointer<CONF>::value || std::is_reference<CONF>::value,
                "Template parameter must be either a pointer or reference type");

 public:
  using Serializable_t = SERIALIZABLE<CONF>;

  static ConfigurationSerializer<CONF, SERIALIZABLE> wrap(CONF conf) { return {conf}; }

  template <class TYPE>
  typename std::enable_if<std::is_same<TYPE, FhiclData>::value, cf::result_t>::type writeDocument(TYPE& data) const
      noexcept try {
    auto tmp = TYPE{""};
    auto result = Serializable_t{_conf}.template writeDocument<TYPE>(tmp);

    if (result) std::swap(data, tmp);

    return {result, {make_error_msg(result ? "Success" : "Failure")}};
  } catch (std::exception const& e) {
    return {false, {make_error_msg(e.what())}};
  }

  template <class TYPE>
  typename std::enable_if<std::is_same<TYPE, JsonData>::value, cf::result_t>::type writeDocument(TYPE& data) const
      noexcept try {
    auto tmp = TYPE{"{}"};
    auto result = Serializable_t{_conf}.template writeDocument<TYPE>(tmp);

    if (result) std::swap(data, tmp);

    return {result, {make_error_msg(result ? "Success" : "Failure")}};
  } catch (std::exception const& e) {
    return {false, {make_error_msg(e.what())}};
  }

  template <class TYPE>
  typename std::enable_if<std::is_same<TYPE, FhiclData>::value, cf::result_t>::type readDocument(
      TYPE const& data) noexcept try {
    auto result = Serializable_t{_conf}.template readDocument<TYPE>(data);

    return {result, {make_error_msg(result ? "Success" : "Failure")}};
  } catch (std::exception const& e) {
    return {false, {make_error_msg(e.what())}};
  }

  template <class TYPE>
  typename std::enable_if<std::is_same<TYPE, JsonData>::value, cf::result_t>::type readDocument(
      TYPE const& data) noexcept try {
    auto result = Serializable_t{_conf}.template readDocument<TYPE>(data);

    return {result, {make_error_msg(result ? "Success" : "Failure")}};
  } catch (std::exception const& e) {
    return {false, {make_error_msg(e.what())}};
  }

  std::string configurationName() const noexcept try {
    return Serializable_t{_conf}.configurationName();
  } catch (std::exception const&) {
    using configurationClazz = typename std::remove_pointer<typename std::remove_reference<CONF>::type>::type;

    auto name = demangle(typeid(configurationClazz).name());

    std::size_t found = name.find_last_of(":");

    if (found != std::string::npos) return name.substr(found + 1);

    return name;
  }

  // defaults
  ~ConfigurationSerializer() = default;
  ConfigurationSerializer(ConfigurationSerializer&&) = default;

  // deleted
  ConfigurationSerializer() = delete;
  ConfigurationSerializer(ConfigurationSerializer const&) = delete;
  ConfigurationSerializer& operator=(ConfigurationSerializer const&) = delete;
  ConfigurationSerializer& operator=(ConfigurationSerializer&&) = delete;

 private:
  ConfigurationSerializer(CONF conf) : _conf(conf){};

 private:
  CONF _conf;
};

struct VersionInfo {
  std::string configuration;
  std::string version;
  std::string entity;

  void validate() const {
    confirm(!configuration.empty());

    if (configuration.empty())
      throw artdaq::database::invalid_option_exception("VersionInfo") << "Configuration name is empty";

    confirm(!version.empty());

    if (version.empty()) throw artdaq::database::invalid_option_exception("VersionInfo") << "Version is empty";

    confirm(!entity.empty());

    if (entity.empty()) throw artdaq::database::invalid_option_exception("VersionInfo") << "Entity is empty";
  }
};

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDBIFC_BASE_H_ */
