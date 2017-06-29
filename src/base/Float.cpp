
#include "mixr/base/Float.hpp"

namespace mixr {
namespace base {

IMPLEMENT_EMPTY_SLOTTABLE_SUBCLASS(Float, "float")

EMPTY_COPYDATA(Float)
EMPTY_DELETEDATA(Float)

std::ostream& Float::serialize(std::ostream& sout, const int, const bool) const
{
   sout << val;
   return sout;
}

}
}
