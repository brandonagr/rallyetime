/* ================================================================

 RallyeTimeDG main app classes, runs everything

Brandon Green - 08-08-24
================================================================= */
#ifndef RALLYETIMEDG_H_
#define RALLYETIMEDG_H_

#include "GPS.h"
#include "Util.h"
#include <memory>


//================================================================
class RallyeTimeDG
{
private:
  Params params_;
  bool kill_flag_;

  GPSThread gps_;
  std::auto_ptr<boost::thread> gps_thread_;



public:
  RallyeTimeDG(const std::string& config_file_location);
  ~RallyeTimeDG();

  void run_till_quit();
};






#endif