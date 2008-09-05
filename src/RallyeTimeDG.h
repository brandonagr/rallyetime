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
#include "RallyeData.h"
#include <memory>


#define MAX_PREVIOUS_STATES 5

#define ENABLE_GPS


//================================================================
class RallyeTimeDG
{
private:
  Params params_;
  bool kill_flag_;

  CSpeech voice_;

#ifdef ENABLE_GPS
  GPSThread gps_;
  boost::thread gps_thread_;
#endif
  GPSData last_gps_pos_;
  
  DAQLCDThread lcd_;
  boost::thread lcd_thread_;
  DAQButtonThread input_;
  boost::thread input_thread_;
  LogManager log_;
  boost::thread log_thread_;
  
  PrettyTime official_time_;

  LCDScreen screen_;
  KeyboardInput key_input_;

  
  std::list<RallyeState> rallye_states_; //front is currently active
  void save_state()
  {
    RallyeState copy(rallye_states_.front());
    rallye_states_.push_front(copy);

    if (rallye_states_.size()>MAX_PREVIOUS_STATES)
      rallye_states_.pop_back();
  }


                     //input official time, input rallye directions, staging(waiting in line to start rallye), running rallye
  enum ProgramState {SETUP_ONE, SETUP_TWO, STAGING, RALLYE};
  ProgramState cur_state_;

  void switch_to_setup_one();
  void update_setup_one(double dt);

  void switch_to_setup_two();
  void update_setup_two(double dt);

  void switch_to_staging();
  void update_staging(double dt);

  void switch_to_rallye();
  void rallye(double dt);


public:
  RallyeTimeDG(const std::string& config_file_location);
  ~RallyeTimeDG();

  void run_till_quit();
};






#endif