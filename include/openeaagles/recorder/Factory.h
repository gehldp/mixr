//------------------------------------------------------------------------------
// Class: Factory
//
// Description: Class factory
//------------------------------------------------------------------------------
#ifndef __oe_Recorder_Factory_H__
#define __oe_Recorder_Factory_H__

namespace oe {

namespace Basic { class Object; }

namespace Recorder {

class Factory
{
public:
   static Basic::Object* createObj(const char* name);

protected:
   Factory();   // prevent object creation
};

}  // end namespace Recorder
}  // end namespace oe

#endif
