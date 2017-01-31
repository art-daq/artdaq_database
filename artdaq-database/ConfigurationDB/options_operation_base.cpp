#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/options_operation_base.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/BasicTypes/basictypes.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/Json/json_writer.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpBase_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace dbbt = db::basictypes;

using dbbt::JsonData;
using cf::OperationBase;
using cf::options::data_format_t;

namespace apiliteral = artdaq::database::configapi::literal;

namespace artdaq {
namespace database {
std::string expand_environment_variables(std::string var);
}
}

OperationBase::OperationBase(std::string const& process_name) : _process_name{process_name} {}

std::string const& OperationBase::operation() const noexcept {
  confirm(!_operation.empty());

  return _operation;
}

std::string const& OperationBase::operation(std::string const& operation) {
  confirm(!operation.empty());

  TRACE_(10, "Options: Updating operation from " << _operation << " to " << operation << ".");

  _operation = operation;

  return _operation;
}

std::string const& OperationBase::collection() const noexcept {
  confirm(!_collection_name.empty());

  return _collection_name;
}

std::string const& OperationBase::collection(std::string const& collection_name) {
  confirm(!collection_name.empty());

  TRACE_(11, "Options: Updating collection_name from " << _collection_name << " to " << collection_name << ".");

  _collection_name = collection_name;

  return _collection_name;
}

std::string const& OperationBase::provider() const noexcept {
  confirm(!_provider.empty());

  return _provider;
}

std::string const& OperationBase::provider(std::string const& provider) {
  confirm(!provider.empty());

  if (db::not_equal(provider, apiliteral::provider::filesystem) &&
      db::not_equal(provider, apiliteral::provider::mongo) && db::not_equal(provider, apiliteral::provider::ucon)) {
    throw db::invalid_option_exception("OperationBase") << "Invalid database provider; database provider=" << provider
                                                        << ".";
  }

  TRACE_(12, "Options: Updating provider from " << _provider << " to " << provider << ".");

  _provider = provider;

  return _provider;
}

data_format_t const& OperationBase::format() const noexcept {
  confirm(_data_format != data_format_t::unknown);
  return _data_format;
}

data_format_t const& OperationBase::format(data_format_t const& data_format) {
  confirm(data_format != data_format_t::unknown);

  TRACE_(13, "Options: Updating data_format from " << cf::to_string(_data_format) << " to "
                                                   << cf::to_string(data_format) << ".");

  _data_format = data_format;

  return _data_format;
}

data_format_t const& OperationBase::format(std::string const& format) {
  confirm(!format.empty());

  TRACE_(14, "Options: format args format=<" << format << ">.");

  _data_format = cf::to_data_format(format);

  return _data_format;
}

std::string const& OperationBase::queryFilter() const noexcept {
  confirm(!_query_payload.empty());

  return _query_payload;
}

std::string const& OperationBase::queryFilter(std::string const& query_payload) {
  confirm(!query_payload.empty());

  auto tmp = db::dequote(query_payload);
  TRACE_(15, "Options: queryFilter args query_payload=<" << tmp << ">.");

  _query_payload = tmp;

  return _query_payload;
}

JsonData OperationBase::query_filter_to_JsonData() const {
  if (queryFilter() != apiliteral::notprovided) {
    return {queryFilter()};
  }

  return {apiliteral::empty_json};
}

bpo::options_description OperationBase::makeProgramOptions() const {
  std::ostringstream descstr;
  descstr << _process_name;
  descstr << " <" << apiliteral::option::searchfilter << ">";

  // descstr << "  <-d <" << apiliteral::option::provider << ">>";
  descstr << "  <-o <" << apiliteral::option::operation << ">>";
  descstr << "  <-f <" << apiliteral::option::format << ">>";
  descstr << "  <-c <" << apiliteral::option::collection << ">>";
  descstr << "  <-r <" << apiliteral::option::result << ">>";

  descstr << " <" << apiliteral::option::searchfilter << ">";

  bpo::options_description opts = descstr.str();

  auto make_opt_name = [](auto& long_name, auto& short_name) {
    return std::string{long_name}.append(",").append(short_name);
  };

  opts.add_options()("help,h", "Produce help message");

  opts.add_options()(make_opt_name(apiliteral::option::operation, "o").c_str(), bpo::value<std::string>(),
                     "Database operation name");
  opts.add_options()(make_opt_name(apiliteral::option::format, "f").c_str(), bpo::value<std::string>(),
                     "In/Out data format [fhicl, xml, gui, db, or csv]");
  opts.add_options()(make_opt_name(apiliteral::option::collection, "c").c_str(), bpo::value<std::string>(),
                     "Configuration collection name");

  opts.add_options()(make_opt_name(apiliteral::option::provider, "d").c_str(), bpo::value<std::string>(),
                     "Database provider name; depricated");

  opts.add_options()(apiliteral::option::searchfilter, bpo::value<std::string>(), "Search filter");

  opts.add_options()(apiliteral::option::searchquery, bpo::value<std::string>(), "Search query");

  opts.add_options()(make_opt_name(apiliteral::option::result, "r").c_str(), bpo::value<std::string>(),
                     "Expected result file name");

  return opts;
}

std::string OperationBase::_getProviderFromURI() {
  auto tmpURI =
      getenv("ARTDAQ_DATABASE_URI") ? db::expand_environment_variables("${ARTDAQ_DATABASE_URI}") : std::string("");

  auto tmpDB = std::string(apiliteral::provider::mongo);
  if (std::equal(std::begin(tmpDB), std::end(tmpDB), tmpURI.begin())) return apiliteral::provider::mongo;

  return apiliteral::provider::filesystem;
}

int OperationBase::readProgramOptions(bpo::variables_map const& vm) {
  if (vm.count("help")) {
    std::cout << makeProgramOptions() << "\n";
    return process_exit_code::HELP;
  }

  if (vm.count(apiliteral::option::searchquery)) {
    auto json = vm[apiliteral::option::searchquery].as<std::string>();
    std::ifstream is(json.c_str());
    if (is.good()) {
      json = std::string((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
      is.close();
    }

    TRACE_(14, "Options: json query= <" << json << ">");

    readJsonData(json);
    return process_exit_code::SUCCESS;
  }

  provider(_getProviderFromURI());

  /*
    if (!vm.count(apiliteral::option::provider)) {
      std::cerr << "Exception from command line processing in " << _process_name
    << ": no datbase provider name given.\n"
                << "For usage and an options list, please do '" << _process_name
    << " --help"
                << "'.\n";
      return process_exit_code::INVALID_ARGUMENT | 1;
    } else {
      provider(vm[apiliteral::option::provider].as<std::string>());
    }
  */

  if (!vm.count(apiliteral::option::operation)) {
    std::cerr << "Exception from command line processing in " << _process_name << ": no database operation given.\n"
              << "For usage and an options list, please do '" << _process_name << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 2;
  } else {
    operation(vm[apiliteral::option::operation].as<std::string>());
  }

  if (vm.count(apiliteral::option::format)) {
    format(vm[apiliteral::option::format].as<std::string>());
  }

  if (vm.count(apiliteral::option::collection)) {
    collection(vm[apiliteral::option::collection].as<std::string>());
  }

  if (vm.count(apiliteral::option::searchfilter)) {
    queryFilter(vm[apiliteral::option::searchfilter].as<std::string>());
  }
  return process_exit_code::SUCCESS;
}

void OperationBase::readJsonData(JsonData const& data) {
  TRACE_(14, "OperationBase::readJsonData() data=<" << data<< ">");
  confirm(!data.empty());
  TRACE_(14, "OperationBase::readJsonData() not empty=<" << data<< ">");
  confirm(data.json_buffer != apiliteral::notprovided);

  TRACE_(14, "OperationBase::readJsonData() object=<" << data<< ">");

  using namespace artdaq::database::json;
  auto filterAST = object_t{};

  if (!JsonReader{}.read(data , filterAST)) {
    TRACE_(14, "OperationBase: JSON buffer= <" << data<< ">");
    throw db::invalid_option_exception("OperationBase") << "OperationBase: Unable to read JSON buffer.";
  }

  try {
    provider(_getProviderFromURI());
    // provider(boost::get<std::string>(filterAST.at(apiliteral::option::provider)));
  } catch (...) {
  }

  try {
    operation(boost::get<std::string>(filterAST.at(apiliteral::option::operation)));
  } catch (...) {
  }

  try {
    format(boost::get<std::string>(filterAST.at(apiliteral::option::format)));
  } catch (...) {
  }

  try {
    collection(boost::get<std::string>(filterAST.at(apiliteral::option::collection)));
  } catch (...) {
  }

  try {
    auto const& filter_object = boost::get<jsn::object_t>(filterAST.at(apiliteral::option::searchfilter));
    auto filter_string = std::string{};
    if (!filter_object.empty() && JsonWriter{}.write(filter_object, filter_string)) queryFilter(filter_string);
  } catch (...) {
  }
}

JsonData OperationBase::writeJsonData() const {
  using namespace artdaq::database::json;

  auto queryFilterAST = object_t{};

  if (!JsonReader{}.read(query_filter_to_JsonData(), queryFilterAST)) {
    throw db::invalid_option_exception("OperationBase") << "Unable to readquery_filter_to_JsonData().";
  }

  auto docAST = object_t{};

  docAST[apiliteral::option::searchfilter] = queryFilterAST;
  // docAST[apiliteral::option::provider] = provider();
  docAST[apiliteral::option::operation] = operation();
  docAST[apiliteral::option::collection] = collection();
  docAST[apiliteral::option::format] = cf::to_string(format());

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("OperationBase") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

JsonData OperationBase::to_JsonData() const { return {writeJsonData()}; }

std::string OperationBase::to_string() const { return to_JsonData(); }

std::ostream& cf::operator<<(std::ostream& os, OperationBase const& o){
  os << o.to_string();
  return os;
}

OperationBase::operator std::string() const{
  return to_string();
}

//
void cf::debug::options::OperationBase() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::options::OperationBase trace_enable");
}
