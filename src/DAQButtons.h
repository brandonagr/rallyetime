/* ================================================================

 Thread that runs the Button/data acquisition

Brandon Green - 08-08-31
================================================================= */
#ifndef DAQBUTTONS_H_
#define DAQBUTTONS_H_


#include "DAQ.h"
#include "ThreadSupport.h"


//----------------------------------------------------------------
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

//----------------------------------------------------------------
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

  //shared data----
  boost::mutex shared_data_mutex_;
  std::list<ButtonEvent> event_que_;
  WSSCount wss_;
  //---------------
  
  void update_wss();
  void reset_buttons(); //reset debounce circuit

public:
  DAQButtonThread(bool* kill_flag);
  ~DAQButtonThread();

  void run();

  //called by main thread, gets data
  bool is_event();  
  ButtonEvent get_event();
  WSSCount get_wss_count();
};

#endif