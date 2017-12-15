
#include "mixr/base/concurrent/Thread.hpp"

#include "mixr/base/Component.hpp"
#include <iostream>

namespace mixr {
namespace base {

Thread::Thread(Component* const p, const double pri) : parent(p), priority(pri)
{
}

Thread::~Thread()
{
   if (!isTerminated()) {
      terminate();
   }
   closeThread();
}

//------------------------------------------------------------------------------
// Create the thread
//------------------------------------------------------------------------------
bool Thread::create()
{
   bool ok{true};

   // Check for a parent pointer
   if (parent == nullptr) {
      std::cerr << "Thread(" << this << ")::create() -- ERROR: Parent pointer is not set!" << std::endl;
      ok = false;
   }

   // Check for valid priority
   if (priority < 0.0f || priority > 1.0f) {
      std::cerr << "Thread(" << this << ")::create() -- ERROR: Invalid priority! Use [ 0.0 ... 1.0 ]" << std::endl;
      ok = false;
   }

   // Create the thread
   if (ok) ok = createThread();

   if (!ok) {
      std::cerr << "Thread(" << this << ")::create() -- ERROR: Did NOT create the thread!" << std::endl;
   }

   killed = !ok;

   return ok;
}

//-----------------------------------------------------------------------------
// Get functions
//-----------------------------------------------------------------------------
bool Thread::isTerminated() const
{
   return killed;
}

double Thread::getPriority() const
{
   return priority;
}

Component* Thread::getParent()
{
   return parent;
}

const void* Thread::getThreadHandle() const
{
   return theThread;
}

// thread stack size in bytes (or zero if using the default stack size)
std::size_t Thread::getStackSize() const
{
   return stackSize;
}

//-----------------------------------------------------------------------------
// Set functions
//-----------------------------------------------------------------------------

// Set the thread's stack size in bytes (or zero to use the system default size)
bool Thread::setStackSize(const std::size_t size)
{
   stackSize = size;
   return true;
}

// Set the terminated flag
void Thread::setTerminated()
{
   killed = true;
}

}
}
