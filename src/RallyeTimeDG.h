/* ================================================================

 RallyeTimeDG main app classes, runs everything

Brandon Green - 08-08-24
================================================================= */
#ifndef RALLYETIMEDG_H_
#define RALLYETIMEDG_H_

#include "DAQLCD.h"
#include "DAQButtons.h"
#include "GPS.h"
#include "Util.h"
#include "ThreadSupport.h"
#include "LogManager.h"
#include "Speech.h"
#include <memory>


//================================================================
class RallyeTimeDG
{
private:
  Params params_;
  bool kill_flag_;

  CSpeech voice_;

  //GPSThread gps_;
  //boost::thread gps_thread_;

  DAQLCDThread lcd_;
  boost::thread lcd_thread_;
  DAQButtonThread input_;
  boost::thread input_thread_;
  LogManager log_;
  boost::thread log_thread_;

  KeyboardInput key_input_;



public:
  RallyeTimeDG(const std::string& config_file_location);
  ~RallyeTimeDG();

  void run_till_quit();
};






#endif