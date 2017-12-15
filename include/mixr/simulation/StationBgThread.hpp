
#ifndef __mixr_simulation_StationBgThread_H__
#define __mixr_simulation_StationBgThread_H__

#include "mixr/base/concurrent/PeriodicTask.hpp"

namespace mixr {
namespace simulation {

// ---
// Background thread
// ---
class BgThread : public base::PeriodicTask
{
   public: BgThread(base::Component* const parent, const double priority, const double rate);
   private: unsigned long userFunc(const double dt) override;
};

}
}



#endif
