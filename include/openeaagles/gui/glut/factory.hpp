
#ifndef __oe_glut_factory_H__
#define __oe_glut_factory_H__

#include <string>

namespace oe {
namespace base { class Object; }
namespace glut {
base::Object* factory(const std::string&);
}
}

#endif
