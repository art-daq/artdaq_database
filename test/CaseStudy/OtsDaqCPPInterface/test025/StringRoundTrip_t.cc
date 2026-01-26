#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (StringRoundTrip test)

#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <random>
#include <sstream>

#include "../DatabaseConfigurationInterface.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace ots;
using artdaq::database::docrecord::JSONDocument;

namespace {

bool isQuickMode() {
  auto* quick = getenv("QUICK_TEST");
  return quick && (std::string(quick) == "1" || std::string(quick) == "true");
}

std::vector<std::string> loadInputsFromFile(const std::string& filename) {
  std::vector<std::string> inputs;
  std::ifstream file(filename);
  if (!file) return inputs;

  std::stringstream buf;
  buf << file.rdbuf();
  std::string content = buf.str();

  size_t pos = 0;
  while ((pos = content.find("\"input\":", pos)) != std::string::npos) {
    size_t start = content.find('{', pos);
    if (start == std::string::npos) break;

    int depth = 1;
    size_t end = start + 1;
    while (depth > 0 && end < content.size()) {
      if (content[end] == '{') depth++;
      else if (content[end] == '}') depth--;
      else if (content[end] == '"') {
        end++;
        while (end < content.size() && content[end] != '"') {
          if (content[end] == '\\') end++;
          end++;
        }
      }
      end++;
    }
    inputs.push_back(content.substr(start, end - start));
    pos = end;
  }
  return inputs;
}

struct TestFixture {
  TestFixture() : baseVersion((srand(time(nullptr)), rand() % 99999 + 100000)) {
    inputs = loadInputsFromFile("test_cases.json");

    if (isQuickMode() && inputs.size() > 10) {
      std::vector<size_t> idx(inputs.size());
      std::iota(idx.begin(), idx.end(), 0);
      std::shuffle(idx.begin(), idx.end(), std::mt19937{std::random_device{}()});
      idx.resize(inputs.size() / 10);
      std::sort(idx.begin(), idx.end());
      std::vector<std::string> sampled;
      for (auto i : idx) sampled.push_back(inputs[i]);
      inputs = std::move(sampled);
    }

    std::cout << "Loaded " << inputs.size() << " test cases" << (isQuickMode() ? " (QUICK MODE)" : "") << "\n";
  }

  int nextVersion() { return baseVersion + (++versionOffset); }

  const int baseVersion;
  std::vector<std::string> inputs;

 private:
  int versionOffset = 0;
};

}  // namespace

TestFixture fixture;

BOOST_AUTO_TEST_SUITE(string_roundtrip_test)

BOOST_AUTO_TEST_CASE(json_roundtrip_ast_comparison) {
  DatabaseConfigurationInterface ifc;
  int passed = 0, failed = 0;

  for (const auto& inputJson : fixture.inputs) {
    int version = fixture.nextVersion();

    auto writer = std::make_shared<TestConfiguration001>();
    writer->getView().fillFromJSON(inputJson);
    writer->getView().version = version;
    if (ifc.saveActiveVersion(writer.get()) != 0) {
      failed++;
      continue;
    }

    auto reader = std::make_shared<TestConfiguration001>();
    if (ifc.fill(reader.get(), version) != 0) {
      failed++;
      continue;
    }

    JSONDocument inputDoc(inputJson);
    JSONDocument outputDoc(reader->getView()._json);

    if (inputDoc == outputDoc) {
      passed++;
    } else {
      failed++;
      std::cout << "FAIL: " << inputJson.substr(0, 60) << "...\n";
    }
  }

  std::cout << "Passed: " << passed << "/" << fixture.inputs.size() << "\n";
  BOOST_CHECK_EQUAL(failed, 0);
}

BOOST_AUTO_TEST_CASE(multiple_roundtrips_stability) {
  DatabaseConfigurationInterface ifc;
  int stable = 0, unstable = 0;

  for (const auto& inputJson : fixture.inputs) {
    std::vector<std::string> reads;
    std::string current = inputJson;

    for (int round = 0; round < 3; ++round) {
      int version = fixture.nextVersion();
      auto writer = std::make_shared<TestConfiguration001>();
      writer->getView().fillFromJSON(current);
      writer->getView().version = version;
      if (ifc.saveActiveVersion(writer.get()) != 0) break;

      auto reader = std::make_shared<TestConfiguration001>();
      if (ifc.fill(reader.get(), version) != 0) break;
      current = reader->getView()._json;
      reads.push_back(current);
    }

    if (reads.size() == 3) {
      JSONDocument d0(inputJson), d1(reads[0]), d2(reads[1]), d3(reads[2]);
      if (d0 == d1 && d1 == d2 && d2 == d3) {
        stable++;
      } else {
        unstable++;
      }
    }
  }

  std::cout << "Stable: " << stable << "/" << fixture.inputs.size() << "\n";
  BOOST_CHECK_EQUAL(unstable, 0);
}

BOOST_AUTO_TEST_SUITE_END()
