#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/options_operations.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/BasicTypes/basictypes.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/Json/json_writer.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpBulk_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace dbbt = db::basictypes;

namespace apiliteral = artdaq::database::configapi::literal;

using dbbt::JsonData;
using cf::BulkOperations;
using cf::options::data_format_t;

namespace apiliteral = artdaq::database::configapi::literal;

BulkOperations::BulkOperations(std::string const& process_name) : _process_name{process_name} {}

std::string const& BulkOperations::bulkOperations() const {
  confirm(!_bulk_operations.empty());

  return _bulk_operations;
}

data_format_t const& BulkOperations::format() const { return _data_format; }

std::string const& BulkOperations::bulkOperations(std::string const& query_payload) {
  confirm(!query_payload.empty());

  auto tmp = db::dequote(query_payload);
  TLOG(15)<< "Options: bulkOperations args query_payload=<" << tmp << ">.";

  _bulk_operations = tmp;

  return _bulk_operations;
}

bpo::options_description BulkOperations::makeProgramOptions() const {
  std::ostringstream descstr;
  descstr << _process_name;
  descstr << " <" << apiliteral::option::bulkoperations << ">";

  descstr << "  <-x <" << apiliteral::option::result << ">>";

  bpo::options_description opts = descstr.str();

  auto make_opt_name = [](auto& long_name, auto& short_name) {
    return std::string{long_name}.append(",").append(short_name);
  };

  opts.add_options()("help,h", "Produce help message");

  opts.add_options()(apiliteral::option::bulkoperations, bpo::value<std::string>(), "BulkOperations json");

  opts.add_options()(make_opt_name(apiliteral::option::result, "x").c_str(), bpo::value<std::string>(),
                     "Expected result file name");

  return opts;
}

int BulkOperations::readProgramOptions(bpo::variables_map const& vm) {
  if (vm.count("help")) {
    std::cout << makeProgramOptions() << "\n";
    return process_exit_code::HELP;
  }

  if (vm.count(apiliteral::option::bulkoperations)) {
    auto json = vm[apiliteral::option::bulkoperations].as<std::string>();
    std::ifstream is(json.c_str());
    if (is.good()) {
      json = std::string((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
      is.close();
    }

    TLOG(14) << "BulkOperations: json = <" << json << ">";

    readJsonData(json);
    return process_exit_code::SUCCESS;
  }

  return process_exit_code::SUCCESS;
}

void BulkOperations::readJsonData(JsonData const& data) {
  confirm(!data.empty());

  using cf::ManageDocumentOperation;
  using namespace artdaq::database::json;
  auto dataAST = object_t{};

  if (!JsonReader{}.read(data, dataAST)) {
    throw db::invalid_option_exception("BulkOperations") << "BulkOperations: Unable to read JSON buffer.";
  }

  bulkOperations(data);

  auto const& operations = boost::get<jsn::array_t>(dataAST.at(apiliteral::operations));

  for (auto& operation : operations) {
    auto const& tmp_op_object = boost::get<jsn::object_t>(operation);
    auto const& opname = boost::get<std::string>(tmp_op_object.at(apiliteral::option::operation));

    std::string buffer = std::string{};

    if (!JsonWriter{}.write(boost::get<jsn::object_t>(operation), buffer)) {
      throw db::invalid_option_exception("BulkOperations") << "Unable to write JSON buffer.";
    }

    _operations_list.emplace_back(std::make_tuple(opname, std::make_unique<ManageDocumentOperation>(_process_name)));

    std::get<OperationBaseUPtr>(_operations_list.back())->readJsonData(buffer);
  }
}

JsonData BulkOperations::to_JsonData() const { return {bulkOperations()}; }

std::string BulkOperations::to_string() const { return to_JsonData(); }

//
void cf::debug::options::BulkOperations() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(0) <<  "artdaq::database::configuration::options::BulkOperations trace_enable";
}
