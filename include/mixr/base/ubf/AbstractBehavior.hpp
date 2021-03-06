
#ifndef __mixr_base_ubf_AbstractBehavior_HPP__
#define __mixr_base_ubf_AbstractBehavior_HPP__

#include "mixr/base/Component.hpp"

namespace mixr {
namespace base {
class Integer;
namespace ubf {
class AbstractState;
class AbstractAction;

//------------------------------------------------------------------------------
// Class: AbstractBehavior
// Description: Abstract base class for all behaviors.  Generates an optional
//              action based on our current state.
//------------------------------------------------------------------------------
// Factory name: UbfBehavior
//------------------------------------------------------------------------------
// Slots:
//    vote     <Integer>   ! default vote/weight value for actions generated
//                         ! by this behavior
//------------------------------------------------------------------------------
class AbstractBehavior : public base::Component
{
   DECLARE_SUBCLASS(AbstractBehavior, base::Component)

public:
   AbstractBehavior();

   // Returns a pre-ref'd Action (or zero if no action is generated)
   virtual AbstractAction* genAction(const AbstractState* const state, const double dt) = 0;

protected:
   int getVote() const;
   virtual void setVote(const int x);

private:
   int vote{};

private:
   // slot table helper methods
   bool setSlotVote(const base::Integer* const);
};

inline void AbstractBehavior::setVote(const int x)    { vote = x; }
inline int AbstractBehavior::getVote() const          { return vote; }

}
}
}

#endif
