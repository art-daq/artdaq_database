#include "artdaq-database/BuildInfo/process_exceptions.h"
#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/options_operation_base.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

#include "artdaq-database/DataFormats/common/shared_literals.h"

#include "artdaq-database/BasicTypes/basictypes.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/Json/json_writer.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpBase_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfl = cf::literal;
namespace dbbt = db::basictypes;

using dbbt::JsonData;
using cf::OperationBase;
using cf::options::data_format_t;

OperationBase::OperationBase(std::string const& process_name) : _process_name{process_name} {}

std::string const& OperationBase::operation() const noexcept {
  assert(!_operation.empty());

  return _operation;
}

std::string const& OperationBase::operation(std::string const& operation) {
  assert(!operation.empty());

  TRACE_(10, "Options: Updating operation from " << _operation << " to " << operation << ".");

  _operation = operation;

  return _operation;
}

std::string const& OperationBase::collectionName() const noexcept {
  assert(!_collection_name.empty());

  return _collection_name;
}

std::string const& OperationBase::collectionName(std::string const& collection_name) {
  assert(!collection_name.empty());

  TRACE_(11, "Options: Updating collection_name from " << _collection_name << " to " << collection_name << ".");

  _collection_name = collection_name;

  return _collection_name;
}

std::string const& OperationBase::provider() const noexcept {
  assert(!_provider.empty());

  return _provider;
}

std::string const& OperationBase::provider(std::string const& provider) {
  assert(!provider.empty());

  if (cf::not_equal(provider, cfl::database_provider_filesystem) &&
      cf::not_equal(provider, cfl::database_provider_mongo)) {
    throw db::invalid_option_exception("Options") << "Invalid database provider; database provider=" << provider << ".";
  }

  TRACE_(12, "Options: Updating provider from " << _provider << " to " << provider << ".");

  _provider = provider;

  return _provider;
}

data_format_t const& OperationBase::dataFormat() const noexcept {
  assert(_data_format != data_format_t::unknown);
  return _data_format;
}

data_format_t const& OperationBase::dataFormat(data_format_t const& data_format) {
  assert(data_format != data_format_t::unknown);

  TRACE_(13, "Options: Updating data_format from " << cf::to_string(_data_format) << " to "
                                                   << cf::to_string(data_format) << ".");

  _data_format = data_format;

  return _data_format;
}

data_format_t const& OperationBase::dataFormat(std::string const& format) {
  assert(!format.empty());

  TRACE_(14, "Options: dataFormat args format=<" << format << ">.");

  _data_format = cf::to_data_format(format);

  return _data_format;
}

std::string const& OperationBase::searchFilter() const noexcept {
  assert(!_search_filter.empty());

  return _search_filter;
}

std::string const& OperationBase::searchFilter(std::string const& search_filter) {
  assert(!search_filter.empty());

  TRACE_(15, "Options: searchFilter args search_filter=<" << search_filter << ">.");

  _search_filter = search_filter;

  return _search_filter;
}

JsonData OperationBase::search_filter_to_JsonData() const {
  if (searchFilter() != cfl::notprovided) {
    return {searchFilter()};
  }

  return {cfl::empty_json};
}

bpo::options_description OperationBase::makeProgramOptions() const {
  std::ostringstream descstr;
  descstr << _process_name;
  descstr << " <" << cfl::option::searchquery << ">";

  descstr << "  <-d <" << cfl::option::provider << ">>";
  descstr << "  <-o <" << cfl::option::operation << ">>";
  descstr << "  <-f <" << cfl::option::format << ">>";
  descstr << "  <-c <" << cfl::option::collection << ">>";
  descstr << "  <-r <" << cfl::option::result << ">>";

  descstr << " <" << cfl::option::searchfilter << ">";

  bpo::options_description opts = descstr.str();

  auto make_opt_name = [](auto& long_name, auto& short_name) {
    return std::string{long_name}.append(",").append(short_name);
  };

  opts.add_options()("help,h", "Produce help message");
  opts.add_options()(make_opt_name(cfl::option::provider, "d").c_str(), bpo::value<std::string>(),
                     "Database provider name");
  opts.add_options()(make_opt_name(cfl::option::operation, "o").c_str(), bpo::value<std::string>(),
                     "Database operation name");
  opts.add_options()(make_opt_name(cfl::option::format, "f").c_str(), bpo::value<std::string>(),
                     "In/Out data format [fhicl, gui, or db]");
  opts.add_options()(make_opt_name(cfl::option::collection, "c").c_str(), bpo::value<std::string>(),
                     "Configuration collection name");

  opts.add_options()(cfl::option::searchfilter, bpo::value<std::string>(), "Search filter");

  opts.add_options()(cfl::option::searchquery, bpo::value<std::string>(), "Search query");

  opts.add_options()(make_opt_name(cfl::option::result, "r").c_str(), bpo::value<std::string>(),
                     "Expected result file name");

  return opts;
}

int OperationBase::readProgramOptions(bpo::variables_map const& vm) {
  if (vm.count("help")) {
    std::cout << makeProgramOptions() << "\n";
    return process_exit_code::HELP;
  }

  if (vm.count(cfl::option::searchquery)) {
    auto json = vm[cfl::option::searchquery].as<std::string>();    
    std::ifstream is(json.c_str());    
    if (is.good()) {
      json = std::string((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
      is.close();      
    }

    TRACE_(14, "Options: json query= <" << json << ">");

    readJsonData(json);
    return process_exit_code::SUCCESS;
  }

  if (!vm.count(cfl::option::provider)) {
    std::cerr << "Exception from command line processing in " << _process_name << ": no datbase provider name given.\n"
              << "For usage and an options list, please do '" << _process_name << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  } else {
    provider(vm[cfl::option::provider].as<std::string>());
  }

  if (!vm.count(cfl::option::operation)) {
    std::cerr << "Exception from command line processing in " << _process_name << ": no database operation given.\n"
              << "For usage and an options list, please do '" << _process_name << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 2;
  } else {
    operation(vm[cfl::option::operation].as<std::string>());
  }

  if (vm.count(cfl::option::format)) {
    dataFormat(vm[cfl::option::format].as<std::string>());
  }

  if (vm.count(cfl::option::collection)) {
    collectionName(vm[cfl::option::collection].as<std::string>());
  }

  if (vm.count(cfl::option::searchfilter)) {
    searchFilter(vm[cfl::option::searchfilter].as<std::string>());
  }
  return process_exit_code::SUCCESS;
}

void OperationBase::readJsonData(JsonData const& data) {
  assert(!data.json_buffer.empty());

  using namespace artdaq::database::json;
  auto filterAST = object_t{};

  if (!JsonReader{}.read(data.json_buffer, filterAST)) {
    throw db::invalid_option_exception("Options") << "Unable to read JSON buffer.";
  }

  try {
    provider(boost::get<std::string>(filterAST.at(cfl::option::provider)));
  } catch (...) {
  }

  try {
    operation(boost::get<std::string>(filterAST.at(cfl::option::operation)));
  } catch (...) {
  }

  try {
    dataFormat(boost::get<std::string>(filterAST.at(cfl::option::format)));
  } catch (...) {
  }

  try {
    collectionName(boost::get<std::string>(filterAST.at(cfl::option::collection)));
  } catch (...) {
  }

  try {
    auto const& filter_object = boost::get<jsn::object_t>(filterAST.at(cfl::option::searchfilter));
    auto filter_string = std::string{};
    if (!filter_object.empty() && JsonWriter{}.write(filter_object, filter_string)) searchFilter(filter_string);
  } catch (...) {
  }
}

JsonData OperationBase::writeJsonData() const {
  using namespace artdaq::database::json;

  auto searchFilterAST = object_t{};

  if (!JsonReader{}.read(search_filter_to_JsonData().json_buffer, searchFilterAST)) {
    throw db::invalid_option_exception("Options") << "Unable to readsearch_filter_to_JsonData().";
  }

  auto docAST = object_t{};

  docAST[cfl::option::searchfilter] = searchFilterAST;
  docAST[cfl::option::provider] = provider();
  docAST[cfl::option::operation] = operation();
  docAST[cfl::option::collection] = collectionName();
  docAST[cfl::option::format] = cf::to_string(dataFormat());

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("Options") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

JsonData OperationBase::to_JsonData() const { return {writeJsonData()}; }

std::string OperationBase::to_string() const { return to_JsonData().json_buffer; }

//
void cf::debug::options::enableOperationBase() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::options::OperationBase trace_enable");
}
