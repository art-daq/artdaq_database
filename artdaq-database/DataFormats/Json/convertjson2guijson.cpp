#include "artdaq-database/DataFormats/common.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "artdaq-database/DataFormats/Json/convertjson2guijson.h"
#pragma GCC diagnostic pop
 
#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/Json/json_writer.h"
#include "artdaq-database/DataFormats/Json/json_types_impl.h"

#include <boost/variant/get.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "FHJS:json2gui_C"


using namespace artdaq::database::sharedtypes;
using namespace artdaq::database::json;

using artdaq::database::json::db2gui;
using artdaq::database::json::gui2db;

/*
 * http://stackoverflow.com/questions/21832701/does-json-syntax-allow-duplicate-keys-in-an-object
 * For now, to have a "better" compartibilty with FHICL I'll allow duplicate key
 * names in JSON.
*/

std::string metadata_node_t::_string_value_of(std::string const& name) const {
  confirm(!name.empty());
  confirm(_node.type() == type_t::OBJECT);

  TRACE_(12, "json_db_to_gui() _string_value_of args name <" << name << ">");

  auto const& value_table = _node.value_as<object_t>();
  auto const& type = value_table.at(name);

  return boost::get<std::string>(type);
}

json_node_t metadata_node_t::child(std::string const& name) const {
  confirm(!name.empty());
  confirm(_node.type() == type_t::OBJECT);

  TRACE_(13, "json_db_to_gui() child args name <" << name << ">");

  auto const& value_table = _node.value_as<object_t>();

  try {
    object_t const& child = jcunwrap(value_table.at(name));
    TRACE_(13, "json_db_to_gui() metadata node was found, name=<" << name << ">");
    return {child};
  } catch (...) {
  }

  try {
    object_t const& children = jcunwrap(value_table.at(literal::children));
    object_t const& child = jcunwrap(children.at(name));
    TRACE_(13, "json_db_to_gui() metadata node was found, name=<" << name << ">");
    
    return {child};
  } catch (...) {
  }

  TRACE_(13, "json_db_to_gui() metadata node was not found, name=<" << name << ">");
  
  return {false};
}

bool metadata_node_t::hasMetadata() const { return _node.type() != type_t::NOTSET; }

bool artdaq::database::json_db_to_gui(std::string const& db_json, std::string& gui_json) {
  TRACE_(10, "json_db_to_gui() begin");

  confirm(!db_json.empty());
  confirm(gui_json.empty());

  auto dbAST = object_t{};

  if (!JsonReader{}.read(db_json, dbAST)) {
    throw runtime_error("json_db_to_gui") << "json_db_to_gui: Unable to read JSON buffer.";
  }

  TRACE_(10, "json_db_to_gui() read dbAST");

  auto guiAST = object_t{};
  auto gui_node = json_node_t{guiAST};

  auto& document = unwrap(dbAST).value_as<const object_t>(literal::document);

  try {
    db2gui{{unwrap(document).value_as<const object_t>(literal::data)},
           {unwrap(document).value_as<const object_t>(literal::metadata)}}(gui_node);

    guiAST.back().key = literal::gui_data;

    try {
      guiAST[literal::changelog] = unwrap(dbAST).value_as<std::string>(literal::changelog);
    } catch (...) {
      TRACE_(10, "json_db_to_gui() No changelog in dbAST");
    }

    auto& doc_node = unwrap(dbAST).value_as<object_t>(literal::document);
    doc_node[literal::converted] = object_t{};
    unwrap(doc_node).value_as<object_t>(literal::converted).swap(guiAST);
  } catch (...) {
    TRACE_(10, "json_db_to_gui() Uncaught exception:" << boost::current_exception_diagnostic_information());
    
    throw;
  }

  TRACE_(10, "json_db_to_gui() created gui_node");

  auto json = std::string();

  if (!JsonWriter{}.write(dbAST, json)) {
    throw runtime_error("json_db_to_gui") << "Unable to write JSON buffer.";
  }

  TRACE_(10, "json_db_to_gui() created gui_json");

  gui_json.swap(json);

  return true;
}

void db2gui::operator()(json_node_t& gui_node) const {
  TRACE_(11, "json_db_to_gui() operator() begin");

  auto data_node = data_node_t(_data_node);

  auto metadata_node = metadata_node_t(_metadata_node);

  switch (_data_node.type()) {
    default: {
      TRACE_(11, "json_db_to_gui() operator() switch default");
      break;
    }

    case type_t::VALUE: {
      TRACE_(11, "json_db_to_gui() operator() switch VALUE");
      auto& array = gui_node.value_as<array_t>();
      array.push_back(data_node.value<value_crt>().value);
      break;
    }

    case type_t::DATA: {
      TRACE_(11, "json_db_to_gui() operator() switch DATA");
      auto& array = gui_node.value_as<array_t>();
      array.push_back(object_t{});
      auto& object = unwrap(array.back()).value_as<object_t>();
      TRACE_(11, "json_db_to_gui() operator() switch DATA data");

      object[literal::type] = std::string{literal::unknown};
      object[literal::name] = data_node.value<data_t>().key;

      auto hasMetadata = metadata_node.hasMetadata();
      object[literal::comment] = (hasMetadata ? metadata_node.comment() : std::string{" "});

      auto const& value = data_node.value<data_t>().value;

      auto type_name = std::string(literal::unknown);

      if (value.type() == typeid(const object_t)) {
        type_name = {literal::table};
        auto object_node = json_node_t{object};
        db2gui{{value}, _metadata_node}(object_node);
      } else if (value.type() == typeid(const array_t)) {
        type_name = {literal::sequence};
        auto array_node = json_node_t{object};
        db2gui{{value}, {false}}(array_node);
      } else {
        type_name = {literal::string};
        object[literal::value] = value;
        try {
          object[literal::annotation] = (hasMetadata ? metadata_node.annotation() : std::string{" "});
        } catch (std::out_of_range const& ex) {
          TRACE_(11, "json_db_to_gui() missing annotation for string data; key" << data_node.value<data_t>().key);
          object[literal::annotation] = std::string{literal::whitespace};
        }
      }

      object[literal::type] = (hasMetadata ? metadata_node.typeName() : type_name);

      break;
    }

    case type_t::ARRAY: {
      TRACE_(11, "json_db_to_gui() operator() switch ARRAY");

      if (gui_node.type() == type_t::OBJECT) {
        TRACE_(11, "json_db_to_gui() operator() switch OBJECT is type_t::OBJECT ");

        auto& object = gui_node.value_as<object_t>();
        object.push_back(data_t::make(literal::children, array_t{}));
        auto& array = unwrap(object.at(literal::children)).value_as<array_t>();
        auto array_node = json_node_t{array};

        db2gui{_data_node, _metadata_node}(array_node);
      } else {
        TRACE_(11, "json_db_to_gui() operator() switch ARRAY looping over children");

        auto& array = gui_node.value_as<array_t>();
        auto array_node = json_node_t{array};

        for (value_t const& value : data_node.value<array_t>()) {
          TRACE_(11, "json_db_to_gui() operator() switch ARRAY child value=" << boost::apply_visitor(print_visitor(), value));
          const value_crt valcrt{value};
          db2gui{{valcrt}, {false}}(array_node);
        }
      }

      break;
    }

    case type_t::OBJECT: {
      TRACE_(11, "json_db_to_gui() operator() switch OBJECT");

      if (gui_node.type() == type_t::ARRAY) {
        TRACE_(11, "json_db_to_gui() operator() switch OBJECT is type_t::ARRAY ");

        auto& array = gui_node.value_as<array_t>();
        array.push_back(object_t{});
        auto& object = unwrap(array.back()).value_as<object_t>();
        auto object_node = json_node_t{object};

        db2gui{_data_node, _metadata_node}(object_node);

      } else {
        TRACE_(11, "json_db_to_gui() operator() switch OBJECT looping over children");

        auto& object = gui_node.value_as<object_t>();
        object.push_back(data_t::make(literal::children, array_t{}));
        auto& objects_array = unwrap(object.back().value).value_as<array_t>();
        auto objects_node = json_node_t{objects_array};

        for (data_t const& data : data_node.value<object_t>()) {
          TRACE_(11, "json_db_to_gui() operator() switch OBJECT child name=<" << data.key << ">");
          auto hasMetadata = metadata_node.hasMetadata();
          db2gui{{data}, (hasMetadata ? metadata_node.child(data.key) : json_node_t{false})}(objects_node);
        }
      }

      break;
    }
  }
}

bool artdaq::database::json_gui_to_db(std::string const& gui_json, std::string& db_json) {
  TRACE_(14, "json_gui_to_db() begin");

  confirm(!gui_json.empty());
  confirm(db_json.empty());

  auto guiAST = object_t{};

  if (!JsonReader{}.read(gui_json, guiAST)) {
    throw runtime_error("json_gui_to_db") << "json_db_to_gui: Unable to read JSON buffer.";
  }

  auto& gui_document = unwrap(guiAST).value_as<object_t>(literal::document);
  auto& converted = unwrap(gui_document).value_as<object_t>(literal::converted);

  TRACE_(14, "json_gui_to_db() read guiAST");

  auto dbAST = object_t{};

  dbAST[literal::document] = object_t{};
  auto& db_document = unwrap(dbAST).value_as<object_t>(literal::document);

  db_document[literal::data] = object_t{};
  auto& data = unwrap(db_document).value_as<object_t>(literal::data);

  db_document[literal::metadata] = object_t{};
  auto& metadata = unwrap(db_document).value_as<object_t>(literal::metadata);

  auto data_node = json_node_t(data);
  auto metadata_node = json_node_t(metadata);

  TRACE_(14, "json_gui_to_db() created dbAST");

  try {
    auto const& gui_node = unwrap(converted.at(literal::gui_data)).value_as<array_t>();
    gui2db{gui_node}(data_node, metadata_node);

    try {
      auto changelog_node = unwrap(converted.at(literal::changelog)).value_as<std::string>();
      TRACE_(14, "json_gui_to_db() Found changelog in guiAST <" << changelog_node << ">");
      guiAST[literal::changelog] = value_t{changelog_node};
    } catch (...) {
      TRACE_(14, "json_gui_to_db() No changelog in guiAST");
    }

    unwrap(gui_document).value_as<object_t>(literal::data).swap(data);
    unwrap(gui_document).value_as<object_t>(literal::metadata).swap(metadata);
    unwrap(gui_document).value_as<object_t>(literal::converted) = object_t{};

  } catch (...) {
    TRACE_(14, "json_gui_to_db() Uncaught exception:" << boost::current_exception_diagnostic_information());
    throw;
  }

  TRACE_(14, "json_gui_to_db() created db_node");

  auto json = std::string();

  if (!JsonWriter{}.write(guiAST, json)) {
    throw runtime_error("json_gui_to_db") << "Unable to write JSON buffer.";
  }

  TRACE_(14, "json_gui_to_db() created db_json");

  db_json.swap(json);

  return true;
}

void gui2db::operator()(json_node_t& data_node[[gnu::unused]], json_node_t& metadata_node[[gnu::unused]]) const {
  TRACE_(15, "json_gui_to_db() operator() begin");

  auto gui_node = gui_node_t(_gui_node);

  switch (gui_node.type()) {
    default: {
      TRACE_(15, "json_gui_to_db() operator() switch default");
      break;
    }

    case type_t::ARRAY: {
      TRACE_(15, "json_gui_to_db() operator() switch ARRAY looping over children");

      for (value_t const& value : gui_node.value<array_t>()) {
        TRACE_(15, "json_gui_to_db() operator() switch ARRAY child value=" << boost::apply_visitor(print_visitor(), value));

        if (value.type() == typeid(object_t)) {
          object_t const& child = jcunwrap(value);

          if (child.count(literal::type) == 0) {
            data_node.value_as<array_t>().push_back(value);
            continue;
          }

          std::string type_name = unwrap(child).value_as<const std::string>(literal::type);

          if (type_name.compare(literal::table) == 0 && child.count(literal::value) == 1 && child.count(literal::children) == 0)
            type_name = std::string{literal::string};

          // auto const& node_name = unwrap( child ).value_as<const std::string>(
          // literal::name );

          TRACE_(15, "json_gui_to_db() operator() switch ARRAY child type=" << type_name);

          if (type_name.compare(literal::sequence) == 0 || type_name.compare(literal::array) == 0) {
            auto const& children_node = unwrap(child).value_as<const array_t>(literal::children);
            auto children_data = json_node_t{data_node.makeChild<array_t, object_t>(child)};
            auto children_metadata = json_node_t{metadata_node.makeChild<object_t, object_t>(child)};

            children_metadata.value_as<object_t>()[literal::type] = std::string{literal::sequence};
            children_metadata.value_as<object_t>()[literal::comment] = unwrap(child).value_as<const std::string>(literal::comment);

            try {
              children_metadata.value_as<object_t>()[literal::annotation] = unwrap(child).value_as<const std::string>(literal::annotation);
            } catch (...) {
              children_metadata.value_as<object_t>()[literal::annotation] = std::string(literal::whitespace);
            }

            TRACE_(15, "json_gui_to_db() operator() switch ARRAY sequence");

            gui2db{children_node}(children_data, children_metadata);
          } else if (type_name.compare(literal::table) == 0 || type_name.compare(literal::object) == 0) {
            auto new_metadata_node = json_node_t{metadata_node.makeChild<object_t, object_t>(child)};
            new_metadata_node.value_as<object_t>()[literal::type] = std::string{literal::table};
            new_metadata_node.value_as<object_t>()[literal::comment] = unwrap(child).value_as<const std::string>(literal::comment);
            new_metadata_node.value_as<object_t>()[literal::children] = object_t{};

            auto const& children_node = unwrap(child).value_as<const array_t>(literal::children);

            auto children_metadata = json_node_t{boost::get<object_t>(new_metadata_node.value_as<object_t>().at(literal::children))};
            auto children_data = json_node_t{data_node.makeChild<object_t, object_t>(child)};

            TRACE_(15, "json_gui_to_db() operator() switch ARRAY table");

            gui2db{children_node}(children_data, children_metadata);
          } else {
            auto const& child_value = child.at(literal::value);
            auto const& node_name = unwrap(child).value_as<const std::string>(literal::name);
            TRACE_(15, "json_gui_to_db() operator() switch ARRAY child name= " << node_name
                                                                               << ", value=" << boost::apply_visitor(print_visitor(), child_value));

            auto new_metadata_node = json_node_t{metadata_node.makeChild<object_t, object_t>(child)};
            new_metadata_node.value_as<object_t>()[literal::type] = type_name;
            new_metadata_node.value_as<object_t>()[literal::comment] = unwrap(child).value_as<const std::string>(literal::comment);
            new_metadata_node.value_as<object_t>()[literal::annotation] = unwrap(child).value_as<const std::string>(literal::annotation);

            data_node.value_as<object_t>().push_back(data_t{node_name, child_value});
          }
        } else if (value.type() == typeid(array_t)) {
          data_node.value_as<array_t>().push_back(value);
        } else {
          data_node.value_as<array_t>().push_back(value);
        }
      }

      break;
    }
  }
}

void artdaq::database::json::debug::JSON2GUIJSON() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::JSON2GUIJSON trace_enable");
}  

