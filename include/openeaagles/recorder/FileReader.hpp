
#ifndef __oe_recorder_FileReader_H__
#define __oe_recorder_FileReader_H__

#include "openeaagles/recorder/InputHandler.hpp"

namespace oe {
namespace base { class String; }
namespace recorder {

//------------------------------------------------------------------------------
// Class: FileReader
// Description: Read and parse data records from a file
//
// Factory name: FileReader
// Slots:
//     filename       <String>     ! Data file name (required)
//     pathname       <String>     ! Path to the data file's directory (optional)
//
// Notes
//    1) The data file consists of a sequence of serialized data records
//    that are preceded by 4 bytes that provided the size of each data record
//    in bytes.  The 4 bytes are stored as an ascii string with leading spaces
//    (e.g., " 123")
//------------------------------------------------------------------------------
class FileReader : public InputHandler
{
    DECLARE_SUBCLASS(FileReader, InputHandler)

public:
   static const unsigned int MAX_INPUT_BUFFER_SIZE = 2000;

public:
   FileReader();

   bool isOpen() const;             // Is the data file open?
   bool isFailed() const;           // Did we have an open or read error?

   virtual bool openFile();         // Open the data file
   virtual void closeFile();        // Close the data file

   // File and path names; set before calling openFile()
   virtual bool setFilename(const base::String* const msg);
   virtual bool setPathName(const base::String* const msg);

protected:
   virtual const DataRecordHandle* readRecordImp() override;

private:
   void initData();

   char* ibuf {};                    // Input data buffer

   std::ifstream* sin {};            // Input stream
   const base::String* filename {};  // File name
   const base::String* pathname {};  // Path to the data file's directory
   bool fileOpened {};               // File opened
   bool fileFailed {};               // Open or read failed
   bool firstPassFlg {true};         // First pass flag
};

}
}

#endif
