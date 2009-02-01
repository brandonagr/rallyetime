/* ================================================================

 Thread that runs the Button/data acquisition

Brandon Green - 08-08-31
================================================================= */
#ifndef DAQBUTTONS_H_
#define DAQBUTTONS_H_


#include "DAQ.h"
#include "ThreadSupport.h"
#include "Util.h"


//==============================================================
// panic is a toggle, everything else is momentary button
struct ButtonEvent
{
  //              0x8           0x4         0x2         0x1
  enum EventType {CHKPNT_PRESS, NEXT_PRESS, UNDO_PRESS, PANIC_ENGAGE, PANIC_RELEASE};
  EventType type_;

  ButtonEvent(EventType type)
    :type_(type)
  {}
};

// type for wheel sensor revolution counter
typedef unsigned long WSSCount;
#define INSTANT_HIST_SIZE 50

//===============================================================
class DAQButtonThread
{
private:
  bool* kill_flag_;

  //ports for lcd output
  DAQTask input_;
  DAQTask wss_input_;
  DAQTask reset_;

  double next_db_, chkpnt_db_, undo_db_;
  bool panic_flipped_;


  double rollout_per_pulse_; //in feet
  
  
  WSSCount prev_wss_;

  WSSCount pulse_history_[INSTANT_HIST_SIZE];
  double pulse_dt_history_[INSTANT_HIST_SIZE];
  int cur_history_;
  bool history_valid_;
  

  //shared data----
  boost::mutex shared_data_mutex_;
  std::list<ButtonEvent> event_que_;
  double dist_;  
  double inst_spd_;
  //---------------
  
  void update_wss(double dt);
  void reset_buttons(); //reset debounce circuit

public:
  DAQButtonThread(bool* kill_flag, Params param);
  ~DAQButtonThread();

  void run();

  //called by main thread, gets data
  bool is_event();  
  ButtonEvent get_event();

  double get_distance(); //distance since last time this function was called in feet
  double get_instant_speed(); //instant speed in feet/second
};


//==============================================================
// manage input
class KeyboardInput
{
private:
  unsigned char keydown_[256];
  unsigned char debounce_[256];

  bool time_input_;
  std::string time_;
  void process_time_input();

public:
  KeyboardInput();

  void update();

  bool keypress(int vkey){return keydown_[vkey]!=0 && debounce_[vkey]==0;} //if this key was newly pressed since the last update
  bool keydown(int vkey){return keydown_[vkey]!=0;}

  void start_time_input();
  bool is_done(){return !time_input_;}
  std::string get_time(){return time_;}
  void stop_time_input();
};

#endif