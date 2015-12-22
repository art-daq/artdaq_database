#ifndef _ARTDAQ_DATABASE_CONFIGURATION_H_
#define _ARTDAQ_DATABASE_CONFIGURATION_H_


#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/ConfigurationDB/dataintrospection.h"

namespace artdaq{
namespace database{
namespace configuration{

template <typename TYPE, template <typename TYPE> class SERIALIZER>
class Configuration
{
public:
    struct metadata
    {
      using version_t = std::string;

      version_t version_release;
    };
    
public:  
    void read(std::istream& is) {
        typename SERIALIZER<TYPE>::reader reader {is};
        visitor::runVisitor(reader, _configuration);
    }

    void write(std::ostream& os) {
        typename SERIALIZER<TYPE>::writer writer {os};
        visitor::runVisitor(writer, _configuration);
    }

    static constexpr auto versioned_typename() {
        return boost::core::demangle(typeid(TYPE).name()) + "_" + TYPE::type_version();
    }

private:
    TYPE _configuration;
    metadata _metadata;
};

} //namespace configuration
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATION_H_ */
