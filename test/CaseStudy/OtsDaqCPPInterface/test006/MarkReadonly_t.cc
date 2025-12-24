#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE (MarkReadonly test)

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

#include "../DatabaseConfigurationInterface.h"

using namespace ots;

struct MarkReadonlyTestData {
  MarkReadonlyTestData() {
    srand(time(nullptr));
    baseVersion = rand() % 99999 + 100000;
    std::cout << "=== MARK READONLY TEST SUITE ===\n";
    std::cout << "Base version: " << baseVersion << "\n\n";
  }

  ~MarkReadonlyTestData() { std::cout << "\n=== TEST SUITE COMPLETE ===\n"; }

  int baseVersion;
};

MarkReadonlyTestData fixture;

BOOST_AUTO_TEST_SUITE(mark_readonly_test)

BOOST_AUTO_TEST_CASE(mark_readonly_nonexistent_version) {
  std::cout << "\n=== TEST 1: Mark readonly nonexistent version ===\n";

  auto ifc = DatabaseConfigurationInterface();

  std::shared_ptr<ConfigurationBase> cfg = std::make_shared<TestConfiguration001>();
  cfg->getView().version = 999999998;

  auto result = ifc.markActiveVersionReadonly(cfg.get());
  std::cout << "Step 1: markActiveVersionReadonly on nonexistent version returned " << result << "\n";

  BOOST_CHECK_NE(result, 0);
  std::cout << "  Verified: Marking nonexistent version returns error\n";
}

BOOST_AUTO_TEST_SUITE_END()
