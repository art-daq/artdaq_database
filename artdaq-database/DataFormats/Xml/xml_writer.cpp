#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/DataFormats/Xml/convertxml2json.h"
#include "artdaq-database/DataFormats/Xml/xml_writer.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "XML:XmlWriter_C"

namespace xml = artdaq::database::xml;
namespace jsn = artdaq::database::json;
namespace pt = boost::property_tree;

using artdaq::database::xml::XmlWriter;

namespace literal = artdaq::database::dataformats::literal;

template <typename T>
bool is_leaf_type(T& leaf) {
  confirm(!leaf.value.empty());
  return jsn::type(leaf.value) == jsn::type_t::VALUE;
}

template <typename T>
std::string leaf_string_value(T& leaf) {
  confirm(!leaf.value.empty());
  return boost::apply_visitor(jsn::tostring_visitor(), leaf.value);
}

bool XmlWriter::write(jsn::object_t const& json_object, std::string& out) {
  confirm(out.empty());
  confirm(!json_object.empty());

  TLOG(12) << "write() begin";

  try {
    auto const& json_tree = boost::get<jsn::object_t>(json_object.at(literal::data));

    pt::ptree xml_tree;

    std::function<void(jsn::object_t const&, pt::ptree&)> convert;

    convert = [&convert](jsn::object_t const& json_tree, pt::ptree& xml_tree) {
      for (auto const& json_branch : json_tree) {
        if (is_leaf_type(json_branch)) {
          xml_tree.add(json_branch.key, leaf_string_value(json_branch));
          continue;
        }

        auto& xml_branch = xml_tree.add_child(json_branch.key, {});
        auto const& json_branch_value = boost::get<jsn::object_t>(json_branch.value);
        convert(json_branch_value, xml_branch);
      }
    };

    convert(json_tree, xml_tree);

    std::ostringstream sout;
    pt::write_xml(sout, xml_tree, pt::xml_writer_make_settings<std::string>(' ', 4));

    auto buffer = sout.str();

    if (buffer.empty()) return false;

    out.swap(buffer);

    TLOG(12) << "write() end";

    return true;
  } catch (std::exception const& e) {
    TLOG(12) << "read() Caught exception message=" << e.what();

    std::cerr << "Caught exception message=" << e.what() << "\n";
    throw;
  }
}

void artdaq::database::xml::debug::XmlWriter() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) <<  "artdaq::database::xml::XmlWriter trace_enable";
}
