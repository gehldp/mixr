//------------------------------------------------------------------------------
// Class: TcpHandler
//------------------------------------------------------------------------------
#ifndef __oe_base_TcpHandler_H__
#define __oe_base_TcpHandler_H__

#include "openeaagles/base/nethandlers/PosixHandler.h"

namespace oe {
namespace basic {

class String;

//------------------------------------------------------------------------------
// Class: TcpHandler
//
// Description: General TCP/IP support class.
//------------------------------------------------------------------------------
class TcpHandler : public PosixHandler
{
   DECLARE_SUBCLASS(TcpHandler, PosixHandler)

public:
   TcpHandler();

   bool hasBeenTerminated()          { return connectionTerminated; }

   bool sendData(const char* const packet, const int size) override;
   unsigned int recvData(char* const packet, const int maxSize) override;
   bool isConnected() const override;
   bool closeConnection() override;

   // "Socket Only" constructor that will be used by one of our derived classes;
   // The socket should be an AF_INET, SOCK_STREAM type socket that has been
   // initialized, bound, connected, and is ready for use.
   TcpHandler(const LcSocket socketNum);

protected:
   bool init() override;

   bool connected;              // true if we are connected
   bool connectionTerminated;   // true if we had a connection, then lost it (or terminated it ourselves)

   bool  noWait;                // TCP's no wait I/O flag
};

} // End basic namespace
} // End oe namespace

#endif
