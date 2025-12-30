#include "test/common.h"

#include "artdaq-database/DataFormats/Json/json_common.h"

#include <fstream>
#include <sstream>

namespace jsn = artdaq::database::json;

// Helper function to write content to a file
bool write_to_file(const std::string& filename, const std::string& content) {
  std::ofstream outfile(filename);

  if (!outfile.is_open()) {
    std::cerr << "ERROR: Failed to open file for writing: " << filename << "\n";
    return false;
  }

  outfile << content;
  outfile.close();

  std::cout << "Debug file written: " << filename << "\n";
  return true;
}

int main(int argc, char* argv[]) try {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <json-file>\n";
    return process_exit_code::INVALID_ARGUMENT;
  }

  auto file_name = std::string{argv[1]};

  // Read input file
  std::ifstream is(file_name);
  if (!is.is_open()) {
    std::cerr << "ERROR: Failed to open input file: " << file_name << "\n";
    return process_exit_code::FAILURE;
  }

  std::stringstream json_buffer;
  json_buffer << is.rdbuf();
  is.close();

  auto original_json = json_buffer.str();

  // Parse original JSON into first AST
  auto reader = jsn::JsonReader{};
  jsn::object_t ast1;

  if (!reader.read(original_json, ast1)) {
    std::cerr << "Failed reading JSON file into AST1: << " << original_json << ">>\n";
    return process_exit_code::FAILURE;
  }

  // Write AST1 to string buffer
  auto writer = jsn::JsonWriter{};
  auto serialized_json = std::string{};

  if (!writer.write(ast1, serialized_json)) {
    std::cerr << "Failed writing AST1 to string buffer\n";
    return process_exit_code::FAILURE;
  }

  // Parse serialized JSON into second AST
  jsn::object_t ast2;

  if (!reader.read(serialized_json, ast2)) {
    std::cerr << "Failed reading serialized JSON into AST2: << " << serialized_json << ">>\n";
    return process_exit_code::FAILURE;
  }

  // Compare both ASTs
  auto compare_result = ast1 == ast2;

  if (compare_result.first) {
    std::cout << "SUCCESS: ASTs are equivalent after roundtrip.\n";
    std::cout << "Original:\n" << original_json << "\n";
    std::cout << "Serialized:\n" << serialized_json << "\n";
    return process_exit_code::SUCCESS;
  }

  // Test failed - output diagnostic information
  std::cout << "Test failed (AST1 != AST2); error message: " << compare_result.second << "\n";

  std::cout << "Original JSON (from file):\n" << original_json << "\n";
  std::cout << "Serialized JSON (from AST1):\n" << serialized_json << "\n";

  // Write debug files in current directory
  write_to_file("ValidateJson.output.json", serialized_json);
  write_to_file("ValidateJson.expected.json", original_json);

  return process_exit_code::FAILURE;
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
