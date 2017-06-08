
#ifndef __mxrp_instruments_factory_H__
#define __mxrp_instruments_factory_H__

#include <string>

namespace mxrp {
namespace base { class Object; }
namespace instruments {
base::Object* factory(const std::string&);
}
}

#endif