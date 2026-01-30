#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (StringLineRoundTrip test)

#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <numeric>
#include <random>

#include "../DatabaseConfigurationInterface.h"
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace ots;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::fhicl::from_json_string;
using artdaq::database::fhicl::to_json_string;

namespace {

bool isQuickMode() {
  auto* q = getenv("QUICK_TEST");
  return q && (std::string(q) == "1" || std::string(q) == "true");
}

std::vector<std::string> loadLines(const std::string& filename) {
  std::vector<std::string> lines;
  std::ifstream file(filename);
  std::string line;
  while (std::getline(file, line))
    if (!line.empty()) lines.push_back(line);
  return lines;
}

std::string wrapInJson(const std::string& value) { return "{\"value\": \"" + to_json_string(value) + "\"}"; }

std::string extractValue(const std::string& json) { return from_json_string(JSONDocument(json).findChild("value").value()); }

struct TestFixture {
  TestFixture() : baseVersion((srand(time(nullptr)), rand() % 99999 + 100000)) {
    lines = loadLines("test_cases.txt");

    if (isQuickMode() && lines.size() > 10) {
      std::vector<size_t> idx(lines.size());
      std::iota(idx.begin(), idx.end(), 0);
      std::shuffle(idx.begin(), idx.end(), std::mt19937{std::random_device{}()});
      idx.resize(lines.size() / 10);
      std::sort(idx.begin(), idx.end());
      std::vector<std::string> sampled;
      for (auto i : idx) sampled.push_back(lines[i]);
      lines = std::move(sampled);
    }

    std::cout << "Loaded " << lines.size() << " test lines" << (isQuickMode() ? " (QUICK MODE)" : "") << "\n";
  }

  int nextVersion() { return baseVersion + (++versionOffset); }

  const int baseVersion;
  std::vector<std::string> lines;

 private:
  int versionOffset = 0;
};

}  // namespace

TestFixture fixture;

BOOST_AUTO_TEST_SUITE(string_line_roundtrip_test)

BOOST_AUTO_TEST_CASE(json_roundtrip_ast_comparison) {
  DatabaseConfigurationInterface ifc;
  int passed = 0, failed = 0;

  for (const auto& line : fixture.lines) {
    int version = fixture.nextVersion();
    std::string inputJson = wrapInJson(line);

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

    std::string outputJson = reader->getView()._json;
    std::string extractedValue = extractValue(outputJson);

    bool astMatch = (JSONDocument(inputJson) == JSONDocument(outputJson));
    bool verbatimMatch = (extractedValue == line);

    if (astMatch && verbatimMatch) {
      passed++;
    } else {
      failed++;
      std::cout << "FAIL: " << line.substr(0, 50) << (line.size() > 50 ? "..." : "") << " [AST:" << (astMatch ? "OK" : "FAIL")
                << " Verbatim:" << (verbatimMatch ? "OK" : "FAIL") << "]\n";
    }
  }

  std::cout << "Passed: " << passed << "/" << fixture.lines.size() << "\n";
  BOOST_CHECK_EQUAL(failed, 0);
}

BOOST_AUTO_TEST_CASE(multiple_roundtrips_stability) {
  DatabaseConfigurationInterface ifc;
  int stable = 0, unstable = 0;

  for (const auto& line : fixture.lines) {
    std::string current = wrapInJson(line);
    std::vector<std::string> reads;

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
      JSONDocument d0(wrapInJson(line)), d1(reads[0]), d2(reads[1]), d3(reads[2]);
      bool astStable = (d0 == d1 && d1 == d2 && d2 == d3);
      bool verbatimMatch = (extractValue(reads[0]) == line);

      if (astStable && verbatimMatch) {
        stable++;
      } else {
        unstable++;
        std::cout << "UNSTABLE: " << line.substr(0, 40) << "..."
                  << " [AST:" << (astStable ? "OK" : "FAIL") << " Verbatim:" << (verbatimMatch ? "OK" : "FAIL") << "]\n";
      }
    }
  }

  std::cout << "Stable: " << stable << "/" << fixture.lines.size() << "\n";
  BOOST_CHECK_EQUAL(unstable, 0);
}

BOOST_AUTO_TEST_SUITE_END()
