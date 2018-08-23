#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Xml/convertxml2json.h"
#include "artdaq-database/DataFormats/Xml/xml_reader.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "xml_reader.cpp"

namespace jsn = artdaq::database::json;

namespace pt = boost::property_tree;

using artdaq::database::xml::XmlReader;

namespace literal = artdaq::database::dataformats::literal;

bool XmlReader::read(std::string const& in, jsn::object_t& json_object) {
  confirm(!in.empty());
  confirm(json_object.empty());

  TLOG(12) << "read() begin";

  try {
    auto object = jsn::object_t();
    object[literal::data] = jsn::object_t();

    auto& json_tree = boost::get<jsn::object_t>(object.at(literal::data));

    pt::ptree xml_tree;
    std::istringstream sin(in);
    pt::read_xml(sin, xml_tree);

    std::function<void(pt::ptree const&, jsn::object_t&)> convert;

    convert = [&convert](pt::ptree const& xml_tree, jsn::object_t& json_tree) {
      for (auto const& xml_branch : xml_tree) {
        if (xml_branch.second.empty()) {
          json_tree.push_back({xml_branch.first, std::string(xml_branch.second.data())});
          continue;
        }

        json_tree.push_back({xml_branch.first, jsn::object_t{}});
        auto& json_branch = boost::get<jsn::object_t>(json_tree.back().value);
        convert(xml_branch.second, json_branch);
      }
    };

    convert(xml_tree, json_tree);

    if (json_tree.empty()) {
      return false;
    }

    json_object.swap(object);

    TLOG(12) << "read() end";

    return true;

  } catch (std::exception const& e) {
    TLOG(12) << "read() Caught exception message=" << e.what();

    std::cerr << "Caught exception message=" << e.what() << "\n";
    throw;
  }
}

void artdaq::database::xml::debug::XmlReader() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::xml::XmlReader trace_enable";
}
