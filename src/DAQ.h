/* ================================================================

 Generic classes for DAQ

Brandon Green - 08-08-24
================================================================= */
#ifndef DAQ_H_
#define DAQ_H_


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

#endif