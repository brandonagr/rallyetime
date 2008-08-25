/* ================================================================

 Run the GPS monitor thread

Brandon Green - 08-08-24
================================================================= */
#ifndef GPS_H_
#define GPS_H_

#include "Util.h"

#include <boost/thread/thread.hpp>
#pragma comment(lib, "boost_thread-vc80-mt-1_35.lib")


//================================================================
class GPSThread
{
private:
  bool* kill_flag_;




public:
  GPSThread(Params& params, bool* kill_flag);
  ~GPSThread();
  GPSThread(const GPSThread& p){std::cout<<"WHATS GOING ON?"<<std::endl; kill_flag_=p.kill_flag_; };

  void run();
};


#endif
