#ifndef _ARTDAQ_DATABASE_CLASS_INTROSPECTION_H_
#define _ARTDAQ_DATABASE_CLASS_INTROSPECTION_H_

#include "artdaq-database/JsonConvert/introspection_impl_fusion.h"

namespace artdaq {
namespace database {
namespace jsonconvert {
namespace visitor {

template <typename Visitor, typename UserData>
struct VisitorApplication : public detail::select_application<Visitor, UserData>::type {};

template <typename Visitor, typename UserData>
void runVisitor(Visitor& visitor, UserData& data) {
  visitor::VisitorApplication<Visitor, UserData>::handle(visitor, data);
}

}  // namespace visitor
}  // namespace jsonconvert
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_DATAINTROSPECTION_H_ */
