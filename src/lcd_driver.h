/* ================================================================

 LCDDriver class that manages and displays data on an LCD screen
 through a NI 6501

Brandon Green - 08-05-29
================================================================= */
#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include "NIDAQmx.h"
#include <string>

//----------------------------------------------------------------
struct DAQException : public std::exception
{
  std::string m_;
  
  DAQException(std::string& message)
    :m_(message)
  {
  }
};

void err_chk(int ret); //global utility function to throw DAQException if the return value of a DAQ funtion is an error


//----------------------------------------------------------------
// encapsulate task to make sure it always destructs properly
struct DAQTask
{
  TaskHandle t_;

  DAQTask(const char taskname[])
    :t_(0)
  {
    err_chk( DAQmxCreateTask(taskname, &t_) );
  }
  ~DAQTask()
  {
    if (t_)
    {
      DAQmxStopTask(t_);
      DAQmxClearTask(t_);
      t_=0;
    }
  }
};


//----------------------------------------------------------------
class LCDDriver
{
private:
  //ports for lcd output
  DAQTask p0_;
  DAQTask p1_;

  void write_data(unsigned char data, bool rs=false);
  void write_string(std::string line);

public:
  LCDDriver();
  ~LCDDriver();



};


#endif