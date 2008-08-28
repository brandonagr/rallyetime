/* ================================================================

 Thread that runs the LCD display

Brandon Green - 08-08-27
================================================================= */
#ifndef DAQLCD_H_
#define DAQLCD_H_


#include "DAQ.h"
#include "ThreadSupport.h"
#include <list>

//----------------------------------------------------------------
struct LCDString
{
  unsigned char x_, y_;
  std::string data_;

  LCDString(unsigned char x, unsigned char y, std::string data)
    :x_(x),
     y_(y),
     data_(data)
  {}
  LCDString()
    :x_(0),
     y_(0)
  {}
};

//----------------------------------------------------------------
class DAQLCDThread
{
private:
  bool* kill_flag_;

  //ports for lcd output
  DAQTask p0_;
  DAQTask p1_;

  //shared data----
  boost::mutex shared_data_mutex_;
  std::list<LCDString> write_que_;
  //---------------
  
  void write_data(unsigned char data, bool rs=false);
  void set_location(unsigned char x, unsigned char y);
  void internal_write_string(LCDString& string);
  

public:
  DAQLCDThread(bool* kill_flag);
  ~DAQLCDThread();

  void run();

  //called by main thread, places string in que
  void write_string(LCDString& data);  
};

#endif