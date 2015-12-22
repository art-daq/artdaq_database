#include "artdaq-database/JsonConvert/json_deserializer.h"

#include <boost/foreach.hpp>

void debugInfo(boost::property_tree::ptree& pt)
{
    BOOST_FOREACH( boost::property_tree::ptree::value_type const& rowPair, pt.get_child( "" ) ) 
    {
        std::cout << rowPair.first << ": " <<  std::endl;
        BOOST_FOREACH( boost::property_tree::ptree::value_type const& itemPair, rowPair.second ) 
        {
            std::cout << "\t" << itemPair.first << " ";
            BOOST_FOREACH( boost::property_tree::ptree::value_type const& node, itemPair.second ) 
            {
                std::cout << node.second.get_value<std::string>() << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}