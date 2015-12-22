#ifndef _ARTDAQ_DATABASE_JSON_READER_H_
#define _ARTDAQ_DATABASE_JSON_READER_H_

#include "artdaq-database/JsonConvert/common.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

void debugInfo(boost::property_tree::ptree&);

namespace artdaq {
namespace database {
namespace jsonconvert {

struct JsonDeserializer {
    typedef std::string result_type;

    explicit JsonDeserializer(std::istream& is): doc {std::move(read_json(is))} { path.reserve(100);}

    void start_member(std::string const& name [[gnu::unused]]) {
        assert(!name.empty());

        path.push_back(name);
    }

    void finish_member(std::string const& name [[gnu::unused]]) {
        assert(!name.empty());

        path.pop_back();
    }

    template <typename Value> void value(Value& value) {
        //std::cout << "\n Reading " << typeid(value).name() << " "  << path.back() << "=";

        if (array_elemnt_index < 0) {
            value = subtree.get<Value>(path.back());
        } else {
            int index = array_elemnt_index++;

            for (auto const & array_elemnt : subtree.get_child(path.back())) {
                if (0 < index--) continue;

                value = boost::lexical_cast<Value>(array_elemnt.second.data());
                break;
            }
        }

        //std::cout << value ;
    }

    void start_object() {
        subtree = doc;

        if (path.empty())
            return;

        for (auto const & name : path) {
            std::cout << "\n" << name << ",";
            subtree = subtree.get_child(name);
        }
    }
    void finish_object() { }

    void start_array()  {
        array_elemnt_index = 0;
    }
    void finish_array() {
        array_elemnt_index = -1;
    }

private:
    boost::property_tree::ptree read_json(std::istream& is) {
        boost::property_tree::ptree doc;
        boost::property_tree::read_json(is, doc);
        // debugInfo(doc);
        return doc;
    }

private:
    boost::property_tree::ptree doc;
    std::vector<std::string> path;
    boost::property_tree::ptree subtree;
    int array_elemnt_index = -1;
};

} //namespace json
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_JSON_READER_H_ */
