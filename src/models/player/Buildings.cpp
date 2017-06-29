
#include "mixr/models/player/Buildings.hpp"

#include "mixr/base/List.hpp"
#include "mixr/base/units/Angles.hpp"

namespace mixr {
namespace models {

IMPLEMENT_EMPTY_SLOTTABLE_SUBCLASS(Building, "Building")
EMPTY_SERIALIZER(Building)
EMPTY_DELETEDATA(Building)

Building::Building()
{
    STANDARD_CONSTRUCTOR()
    static base::String generic("GenericBuilding");
    setType(&generic);
}

void Building::copyData(const Building& org, const bool)
{
    BaseClass::copyData(org);
}

//-----------------------------------------------------------------------------
// getMajorType() -- Returns the player's major type
//-----------------------------------------------------------------------------
unsigned int Building::getMajorType() const
{
    return BUILDING;
}

}
}
