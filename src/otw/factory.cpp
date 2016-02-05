
#include "openeaagles/otw/factory.h"

#include "openeaagles/basic/Object.h"

#include "openeaagles/otw/OtwCigiCl.h"
#include "openeaagles/otw/OtwPC.h"

#include <cstring>

namespace oe {

namespace otw {

basic::Object* factory(const char* name)
{
    basic::Object* obj = nullptr;

    // Common Image Generation Interface (CIGI)
    if ( std::strcmp(name, OtwCigiCl::getFactoryName()) == 0 ) {
        obj = new OtwCigiCl();
    }
    else if ( std::strcmp(name, CigiClNetwork::getFactoryName()) == 0 ) {
        obj = new CigiClNetwork();
    }

    // PC Visual Driver
    else if ( std::strcmp(name, OtwPC::getFactoryName()) == 0 ) {
        obj = new OtwPC();
    }

    return obj;
}

}
}