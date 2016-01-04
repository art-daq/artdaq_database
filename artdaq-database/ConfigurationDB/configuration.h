#ifndef _ARTDAQ_DATABASE_CONFIGURATION_H_
#define _ARTDAQ_DATABASE_CONFIGURATION_H_

#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/JsonConvert/class_introspection.h"

namespace artdaq{
namespace database{
namespace configuration{

namespace visitor = ::artdaq::database::jsonconvert::visitor;

template <typename CONF, typename SERIALIZER>
class Configuration
{
public:
    struct metadata {
        using version_t = std::string;

        version_t version_release;
    };

public:
  
    Configuration(CONF const& configuration)
    : _configuration(configuration),_metadata{"V001"}{} 
    
    void read(std::istream& is) {
        typename SERIALIZER::reader reader {is};
        visitor::runVisitor(reader, _configuration);
    }

    void write(std::ostream& os) {
        typename SERIALIZER::writer writer {os};
        visitor::runVisitor(writer, _configuration);
    }

    static constexpr auto versioned_typename() {
        return boost::core::demangle(typeid(CONF).name()) + "_" + CONF::type_version();
    }

    CONF& get(){return _configuration;};
    
private:
    CONF _configuration;
    metadata _metadata;
};

} //namespace configuration
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATION_H_ */
