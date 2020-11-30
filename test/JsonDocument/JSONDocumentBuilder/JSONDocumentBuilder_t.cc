#include <fstream>
#include <iostream>
#include <memory>
#include <streambuf>
#include <string>

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"

#include <boost/program_options.hpp>
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

namespace bpo = boost::program_options;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;
namespace ovl = artdaq::database::overlay;

using test_case = bool (*)(const std::string&);

bool test_buildDocument(std::string const& conf);

bool test_addAlias(std::string const& conf);
bool test_setVersion(std::string const& conf);
bool test_removeAlias(std::string const& conf);
bool test_markDeleted(std::string const& conf);
bool test_markReadonly(std::string const& conf);

bool test_addToGlobalConfig(std::string const& conf);

bool test_addEntity(std::string const& conf);
bool test_removeEntity(std::string const& conf);

int main(int argc, char* argv[]) {
  artdaq::database::docrecord::debug::JSONDocument();
  artdaq::database::docrecord::debug::JSONDocumentUtils();

  artdaq::database::docrecord::debug::JSONDocumentBuilder();
  debug::registerUngracefullExitHandlers();

  std::ostringstream descstr;
  descstr << argv[0] << " <-c <config-file>> <other-options>";

  bpo::options_description desc = descstr.str();

  desc.add_options()("config,c", bpo::value<std::string>(), "Configuration file.")("help,h", "produce help message");

  bpo::variables_map vm;

  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error const& e) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": " << e.what() << "\n";
    return -1;
  }

  if (vm.count("help") != 0u) {
    std::cout << desc << std::endl;
    return 1;
  }
  if (vm.count("config") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no configuration file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return 2;
  }

  auto file_name = vm["config"].as<std::string>();

  std::ifstream is(file_name);

  std::string conf((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  auto name = [](auto const& conf) { return JSONDocument(conf).value_as<std::string>("operation"); };

  auto runTest = [](std::string const& name) {
    auto tests = std::map<std::string, test_case>{{"buildDocument", test_buildDocument}, {"addAlias", test_addAlias},
                                                  {"addEntity", test_addEntity},         {"removeAlias", test_removeAlias},
                                                  {"removeEntity", test_removeEntity},   {"markDeleted", test_markDeleted},
                                                  {"markReadonly", test_markReadonly},   {"addToGlobalConfig", test_addToGlobalConfig},
                                                  {"setVersion", test_setVersion}};

    std::cout << "Running test:<" << name << ">\n";

    return tests.at(name);
  };

  auto testResult = runTest(name(conf))(conf);

  return static_cast<int>(!testResult);
}

namespace literal {
constexpr auto operation [[gnu::unused]] = "operation";
constexpr auto path [[gnu::unused]] = "path";
constexpr auto beginstate [[gnu::unused]] = "begin-state";
constexpr auto delta [[gnu::unused]] = "delta";
constexpr auto endstate [[gnu::unused]] = "end-state";
constexpr auto returnedvalue [[gnu::unused]] = "returned-value";
constexpr auto mustsucceed [[gnu::unused]] = "must-succeed";
}  // namespace literal

bool test_buildDocument(std::string const& conf) {
  confirm(!conf.empty());

  auto opts = JSONDocument(conf);
  auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
  auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

  auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

  try {
    // createFromData(begin)
    JSONDocumentBuilder returned{};
    returned.createFromData(begin);
    JSONDocumentBuilder expected{end};

    using namespace artdaq::database::overlay;
    ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS);

    auto result = returned == expected;

    std::cerr << "returned:\n" << returned << "\n";
    std::cerr << "expected:\n" << expected << "\n";

    if (!result.first && mustsucceed) {
      std::cout << "Error returned!=expected.\n";
      std::cerr << "returned:\n" << returned << "\n";
      std::cerr << "expected:\n" << expected << "\n";
      std::cerr << "error:\n" << result.second << "\n";
      return false;
    }

  } catch (cet::exception const& e) {
    if (mustsucceed) {
      throw;
    }
  }

  return true;
}

bool test_addAlias(std::string const& conf) {
  confirm(!conf.empty());

  auto opts = JSONDocument(conf);
  auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
  auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
  auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

  auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

  try {
    JSONDocumentBuilder returned{begin};
    returned.addAlias(delta);
    JSONDocumentBuilder expected{end};

    using namespace artdaq::database::overlay;
    ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS);

    auto result = returned == expected;

    if (!result.first && mustsucceed) {
      std::cout << "Error returned!=expected.\n";
      std::cerr << "returned:\n" << returned << "\n";
      std::cerr << "expected:\n" << expected << "\n";
      std::cerr << "error:\n" << result.second << "\n";
      return false;
    }

  } catch (cet::exception const& e) {
    if (mustsucceed) {
      throw;
    }
  }

  return true;
}

bool test_removeAlias(std::string const& conf) {
  confirm(!conf.empty());

  auto opts = JSONDocument(conf);
  auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
  auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
  auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

  auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

  try {
    JSONDocumentBuilder returned{begin};
    returned.removeAlias(delta);
    JSONDocumentBuilder expected{end};

    using namespace artdaq::database::overlay;
    ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS);

    auto result = returned == expected;

    if (!result.first && mustsucceed) {
      std::cout << "Error returned!=expected.\n";
      std::cerr << "returned:\n" << returned << "\n";
      std::cerr << "expected:\n" << expected << "\n";
      std::cerr << "error:\n" << result.second << "\n";
      return false;
    }

  } catch (cet::exception const& e) {
    if (mustsucceed) {
      throw;
    }
  }

  return true;
}

bool test_addToGlobalConfig(std::string const& conf) {
  confirm(!conf.empty());

  auto opts = JSONDocument(conf);
  auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
  auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
  auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

  auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

  try {
    JSONDocumentBuilder returned{begin};
    returned.addConfiguration(delta);
    JSONDocumentBuilder expected{end};

    using namespace artdaq::database::overlay;
    ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS);

    auto result = returned == expected;

    if (!result.first && mustsucceed) {
      std::cout << "Error returned!=expected.\n";
      std::cerr << "returned:\n" << returned << "\n";
      std::cerr << "expected:\n" << expected << "\n";
      std::cerr << "error:\n" << result.second << "\n";
      return false;
    }

  } catch (cet::exception const& e) {
    if (mustsucceed) {
      throw;
    }
  }

  return true;
}

bool test_setVersion(std::string const& conf) {
  confirm(!conf.empty());

  auto opts = JSONDocument(conf);
  auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
  auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
  auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

  auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

  try {
    JSONDocumentBuilder returned{begin};
    returned.setVersion(delta);
    JSONDocumentBuilder expected{end};

    using namespace artdaq::database::overlay;
    ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS);

    auto result = returned == expected;

    if (!result.first && mustsucceed) {
      std::cout << "Error returned!=expected.\n";
      std::cerr << "returned:\n" << returned << "\n";
      std::cerr << "expected:\n" << expected << "\n";
      std::cerr << "error:\n" << result.second << "\n";
      return false;
    }

  } catch (cet::exception const& e) {
    if (mustsucceed) {
      throw;
    }
  }

  return true;
}

bool test_markReadonly(std::string const& conf) {
  confirm(!conf.empty());

  auto opts = JSONDocument(conf);
  auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
  auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

  auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

  try {
    JSONDocumentBuilder returned{begin};
    returned.markReadonly();
    JSONDocumentBuilder expected{end};

    using namespace artdaq::database::overlay;
    ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS);

    auto result = returned == expected;

    if (!result.first && mustsucceed) {
      std::cout << "Error returned!=expected.\n";
      std::cerr << "returned:\n" << returned << "\n";
      std::cerr << "expected:\n" << expected << "\n";
      std::cerr << "error:\n" << result.second << "\n";
      return false;
    }

  } catch (cet::exception const& e) {
    if (mustsucceed) {
      throw;
    }
  }

  return true;
}

bool test_markDeleted(std::string const& conf) {
  confirm(!conf.empty());

  auto opts = JSONDocument(conf);
  auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
  auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

  auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

  try {
    JSONDocumentBuilder returned{begin};
    returned.markDeleted();
    JSONDocumentBuilder expected{end};

    using namespace artdaq::database::overlay;
    ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS);

    auto result = returned == expected;

    if (!result.first && mustsucceed) {
      std::cout << "Error returned!=expected.\n";
      std::cerr << "returned:\n" << returned << "\n";
      std::cerr << "expected:\n" << expected << "\n";
      std::cerr << "error:\n" << result.second << "\n";
      return false;
    }

  } catch (cet::exception const& e) {
    if (mustsucceed) {
      throw;
    }
  }

  return true;
}

bool test_addEntity(std::string const& conf) {
  confirm(!conf.empty());

  auto opts = JSONDocument(conf);
  auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
  auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
  auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

  auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

  try {
    JSONDocumentBuilder returned{begin};
    returned.addEntity(delta);
    JSONDocumentBuilder expected{end};

    using namespace artdaq::database::overlay;
    ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS);

    auto result = returned == expected;

    if (!result.first && mustsucceed) {
      std::cout << "Error returned!=expected.\n";
      std::cerr << "returned:\n" << returned << "\n";
      std::cerr << "expected:\n" << expected << "\n";
      std::cerr << "error:\n" << result.second << "\n";
      return false;
    }

  } catch (cet::exception const& e) {
    if (mustsucceed) {
      throw;
    }
  }

  return true;
}

bool test_removeEntity(std::string const& conf) {
  confirm(!conf.empty());

  auto opts = JSONDocument(conf);
  auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
  auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
  auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

  auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

  try {
    JSONDocumentBuilder returned{begin};
    returned.removeEntity(delta);
    JSONDocumentBuilder expected{end};

    using namespace artdaq::database::overlay;
    ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS);

    auto result = returned == expected;

    if (!result.first && mustsucceed) {
      std::cout << "Error returned!=expected.\n";
      std::cerr << "returned:\n" << returned << "\n";
      std::cerr << "expected:\n" << expected << "\n";
      std::cerr << "error:\n" << result.second << "\n";
      return false;
    }

  } catch (cet::exception const& e) {
    if (mustsucceed) {
      throw;
    }
  }

  return true;
}
