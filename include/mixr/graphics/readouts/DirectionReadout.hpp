
#ifndef __mixr_graphics_DirectionReadout_HPP__
#define __mixr_graphics_DirectionReadout_HPP__

#include "NumericReadout.hpp"

#include "mixr/graphics/readouts/readout_utils.hpp"

namespace mixr {
namespace graphics {

//------------------------------------------------------------------------------
// Class: DirectionReadout
//------------------------------------------------------------------------------
// EDL Interface:
//
// Factory name: DirectionReadout
// Slots: none
//------------------------------------------------------------------------------
// Notes:
//    1) The 'D' character is used to define the degrees field (required)
//
//    2) The 'M' character is used to define the minutes field (optional, but
//    required when seconds are displayed)
//
//    3) The 'S' character is used to define the seconds field (optional)
//
//    4) The "at symbol" (@) is replaced with a degree symbol (ASCII 0xB0)
//
//    5) The prefix or suffix '+' and '-' characters can be replaced using the
//    'plusChar' and 'minusChar' slots (e.g., to change the '+' to 'E' and
//    the '-' to 'W')
//
// Example formats
//
//    DD.D@          // Degrees
//    0DD.D@         // Degrees with leading zeros
//    +DD.D@         // Degrees with '+' on positive values
//    DD.D@+         // Degrees with the '+' or '-' character as a suffix
//
//    DD@MM          // Degrees and minutes
//    DD@MM.M        // Degrees and minutes
//
//    DD@MM'SS       // Degrees, minutes and seconds
//    DD@MM'SS.S     // Degrees, minutes and seconds
//
//    +0DD@MM'SS.S   // Degrees, minutes and seconds with '+' on positive values
//    0DD@MM'SS.S+   //  ... and with the '+' or '-' character as a suffix
//
//------------------------------------------------------------------------------
class DirectionReadout : public NumericReadout
{
   DECLARE_SUBCLASS(DirectionReadout, NumericReadout)

public:
   DirectionReadout();
   char filterInputEvent(const int event, const char) override;
   double getInputValue() const override;

protected:
   void makeText() override;
   void reformat(const char* const example) override;
   DirMode tmode {DirMode::dd};
};

}
}

#endif
