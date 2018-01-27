
#ifndef __mixr_base_numeric_Boolean_H__
#define __mixr_base_numeric_Boolean_H__

#include "mixr/base/Object.hpp"

#include <iostream>

namespace mixr {
namespace base {

//------------------------------------------------------------------------------
// Class: Boolean
//
// Description: A data type that represents two values (true and false).  This
//              class is defined to support the translation of Boolean type
//              values from the Extensible Description Language (EDL) to C++
//              code that consumes that specification.
//
// Factory name: Boolean
// Slots:
//     value  <Boolean>  ! Sets the value. (default: false)
//
//------------------------------------------------------------------------------
class Boolean : public Object
{
   DECLARE_SUBCLASS(Boolean, Object)

public:
   Boolean()                           { STANDARD_CONSTRUCTOR()            }
   explicit Boolean(const bool x)      { STANDARD_CONSTRUCTOR() value = x; }

   explicit operator bool() const      { return value; }

   void setValue(const bool x)         { value = x;    }
   bool to_bool() const                { return value; }

   // deprecated interface
// bool getBoolean() const             { return value; }

private:
   bool value{};

private:
   // slot table helper methods
   virtual bool setSlotValue(const Boolean* const);
};

}
}

#endif
