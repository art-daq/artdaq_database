#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/ConfigurationDB/dboperations_common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/FhiclJson/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/JSONDocument_template.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpComm_C"

namespace db = artdaq::database;
namespace dbjsu = db::jsonutils;
namespace dbjsul = db::jsonutils::literal;
namespace cf = db::configuration;
namespace cfo = cf::options;
namespace cfol = cfo::literal;
namespace dbbt = db::basictypes;

using dbbt::JsonData;
using dbbt::FhiclData;
using dbjsu::JSONDocument;
using dbjsu::JSONDocumentBuilder;

using cfo::JsonSerializable;
using cfo::data_format_t;

void JsonSerializable::read(std::string const& search_filter) {
  assert(!search_filter.empty());

  auto filter_document = JSONDocument{search_filter};

  _data_format = cfo::data_format_t::json;

  try {
    dataFormat(filter_document.value_as<std::string>(cfol::dataformat));
  } catch (...) {
  }

  try {
    globalConfigurationId(filter_document.value_as<std::string>(cfol::configuration));
  } catch (...) {
  }

  try {
    operation(filter_document.value_as<std::string>(cfol::operation));
  } catch (...) {
  }

  try {
    _search_filter = filter_document.value_as<std::string>(cfol::filter);
  } catch (...) {
  }

  try {
    provider(filter_document.value_as<std::string>(cfol::dbprovider));
  } catch (...) {
  }
}

std::string const& JsonSerializable::operation() const noexcept {
  assert(!_operation.empty());

  return _operation;
}

std::string const& JsonSerializable::operation(std::string const& operation) {
  assert(!operation.empty());

  TRACE_(11, "Options: Updating operation from " << _operation << " to " << operation << ".");

  _operation = operation;

  return _operation;
}

std::string const& JsonSerializable::globalConfigurationId() const noexcept {
  assert(!_global_configuration_id.empty());

  return _global_configuration_id;
}

std::string const& JsonSerializable::globalConfigurationId(std::string const& global_configuration_id) {
  assert(!global_configuration_id.empty());

  if (global_configuration_id.empty()) {
    throw cet::exception("Options") << "Invalid global config id; version is empty.";
  }

  TRACE_(14, "Options: Updating global_configuration_id from " << _global_configuration_id << " to "
                                                               << global_configuration_id << ".");

  _global_configuration_id = global_configuration_id;

  return _global_configuration_id;
}

std::string const& JsonSerializable::provider() const noexcept {
  assert(!_provider.empty());

  return _provider;
}

std::string const& JsonSerializable::provider(std::string const& provider) {
  assert(!provider.empty());

  if (not_equal(provider, cfol::database_provider_filesystem) && not_equal(provider, cfol::database_provider_mongo)) {
    throw cet::exception("Options") << "Invalid database provider; database provider=" << provider << ".";
  }

  TRACE_(14, "Options: Updating provider from " << _provider << " to " << provider << ".");

  _provider = provider;

  return _provider;
}

data_format_t const& JsonSerializable::dataFormat() const noexcept {
  assert(_data_format != data_format_t::unknown);
  return _data_format;
}

data_format_t const& JsonSerializable::dataFormat(data_format_t const& data_format) {
  assert(data_format != data_format_t::unknown);

  TRACE_(15, "Options: Updating data_format from " << decode(_data_format) << " to " << decode(data_format) << ".");

  _data_format = data_format;

  return _data_format;
}

data_format_t const& JsonSerializable::dataFormat(std::string const& format) {
  assert(!format.empty());

  TRACE_(16, "Options: dataFormat args format=<" << format << ">.");

  if (format.compare("fhicl") == 0) {
    dataFormat(data_format_t::fhicl);
  } else if (format.compare("json") == 0) {
    dataFormat(data_format_t::json);
  } else if (format.compare("gui") == 0) {
    dataFormat(data_format_t::gui);
  } else if (format.compare("db") == 0) {
    dataFormat(data_format_t::db);
  } else {
    dataFormat(data_format_t::unknown);
  }

  return _data_format;
}

JSONDocument JsonSerializable::globalConfigurationId_jsndoc() const {
  auto kvp = std::make_pair<std::string, std::string>(dbjsul::configurations, _global_configuration_id.c_str());
  using namespace artdaq::database::jsonutils;

  return toJSONDocument(kvp);
}

std::string const& JsonSerializable::searchFilter() const noexcept {
  assert(!_search_filter.empty());

  return _search_filter;
}

JSONDocument JsonSerializable::search_filter_jsondoc() const {
  if (searchFilter() != cfol::notprovided) {
    return {searchFilter()};
  }

  std::stringstream ss;
  ss << "{";
  if (_global_configuration_id != cfol::notprovided) {
    ss << cf::quoted_(dbjsul::configurations_name) << cfol::colon << cf::quoted_(_global_configuration_id);
  }
  ss << "}";

  return {ss.str()};
}

JSONDocument JsonSerializable::to_jsondoc() const {
  std::stringstream ss;
  ss << "{";
  ss << cf::quoted_(cfol::filter) << cfol::colon << search_filter_jsondoc().to_string();
  ss << ",\n" << cf::quoted_(cfol::dbprovider) << cfol::colon << quoted_(provider());
  ss << ",\n" << cf::quoted_(cfol::operation) << cfol::colon << quoted_(operation());
  ss << ",\n" << cf::quoted_(cfol::dataformat) << cfol::colon << quoted_(cf::decode(dataFormat()));
  ss << "}";

  return {ss.str()};
}

std::string JsonSerializable::to_string() const { return to_jsondoc().to_string(); }

std::string cf::decode(data_format_t const& f) {
  switch (f) {
    default:
    case data_format_t::unknown:
      return "unknown";

    case data_format_t::fhicl:
      return "fhicl";

    case data_format_t::json:
      return "json";

    case data_format_t::gui:
      return "gui";

    case data_format_t::db:
      return "db";
  }
}

bool cf::equal(std::string const& left, std::string const& right) {
  assert(!left.empty());
  assert(!right.empty());

  return left.compare(right) == 0;
}

bool cf::not_equal(std::string const& left, std::string const& right) { return !equal(left, right); }

std::string cf::quoted_(std::string const& text) { return "\"" + text + "\""; }

void cf::trace_enable_CommonOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::configuration::JsonSerializable"
                << "trace_enable");
}
