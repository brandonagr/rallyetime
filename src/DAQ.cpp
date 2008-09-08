#include "DAQ.h"


//----------------------------------------------------------------
// 
void err_chk(int ret)
{
  if (DAQmxFailed(ret))
  {
    char error[1024];
    DAQmxGetExtendedErrorInfo(error,1024);      
    throw DAQException(std::string(error));
  }
}
