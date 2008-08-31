#include "DAQButtons.h"
#include <iostream>

using namespace std;

//----------------------------------------------------------------
// 
DAQButtonThread::DAQButtonThread(bool* kill_flag)
 :kill_flag_(kill_flag),
  input_("task_button_input"),
  wss_input_("task_wheelsensor_input"),
  reset_("task_button_reset"),

  next_db_(0.0),
  chkpnt_db_(0.0),
  undo_db_(0.0),
  panic_flipped_(false)
{
  err_chk( DAQmxCreateDIChan(input_.t_,"Dev1/port1/line4:7","",DAQmx_Val_ChanForAllLines) );
  err_chk( DAQmxCreateCICountEdgesChan(wss_input_.t_,"Dev1/ctr0","",DAQmx_Val_Falling,0,DAQmx_Val_CountUp));
  err_chk( DAQmxCreateDOChan(reset_.t_,"Dev1/port2/line6","",DAQmx_Val_ChanForAllLines) );
  err_chk( DAQmxStartTask (input_.t_) );
  err_chk( DAQmxStartTask (wss_input_.t_) );
  err_chk( DAQmxStartTask (reset_.t_) );

  reset_buttons();
}
DAQButtonThread::~DAQButtonThread()
{

}

//----------------------------------------------------------------
// 
void DAQButtonThread::update_wss()
{
  WSSCount ndata;
  err_chk (DAQmxReadCounterScalarU32(wss_input_.t_,1.0,&ndata,NULL));

  scoped_lock lock(shared_data_mutex_);
  wss_=ndata;
}

//----------------------------------------------------------------
// reset debounce circuit by going high then staying low
void DAQButtonThread::reset_buttons()
{
  unsigned char data_reset=0;
  int32 written;
  err_chk (DAQmxWriteDigitalU8(reset_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_reset,&written,(bool32*)NULL));
  data_reset=0xFF;
  err_chk (DAQmxWriteDigitalU8(reset_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_reset,&written,(bool32*)NULL));
}


//----------------------------------------------------------------
// return if a new button event exists in the que
bool DAQButtonThread::is_event()
{
  scoped_lock lock(shared_data_mutex_);

  return (event_que_.size()>0);
}


//----------------------------------------------------------------
// 
ButtonEvent DAQButtonThread::get_event()
{
  scoped_lock lock(shared_data_mutex_);

  ButtonEvent ndata=event_que_.front();
  event_que_.pop_front();

  return ndata;
}


//----------------------------------------------------------------
// 
WSSCount DAQButtonThread::get_wss_count()
{
  scoped_lock lock(shared_data_mutex_);

  return wss_;
}


//----------------------------------------------------------------
// 
void DAQButtonThread::run()
{
  LARGE_INTEGER freq_, prev_time_, this_time_;   
  double dt=0.0;
  QueryPerformanceFrequency(&freq_);
  double freq_inv_=1/(double)freq_.QuadPart;
  QueryPerformanceCounter(&prev_time_);  

  double daq_bandwidth_limiter=0.0;

  while(!(*kill_flag_))
  {
    //get dt of the loop
    QueryPerformanceCounter(&this_time_);
    dt = double(this_time_.QuadPart-prev_time_.QuadPart) * freq_inv_;
    prev_time_=this_time_;


    daq_bandwidth_limiter+=dt;

    //update any active debounce counters
    next_db_+=dt;
    chkpnt_db_+=dt;
    undo_db_+=dt;
    
    if (daq_bandwidth_limiter>0.1) //only check at 10hz
    {
      //update buttons--------------------------------
      unsigned char data;
      int32 read;
      err_chk (DAQmxReadDigitalU8 (input_.t_,1,1.0,DAQmx_Val_GroupByChannel,&data,1,&read,(bool32*)NULL));
      data=((data>>4)^3);
      
      if (data>3)
        reset_buttons();

      //CHKPNT_PRESS
      if (data&0x8)
      {
        if (chkpnt_db_==0.0 || chkpnt_db_>0.5)
        {            
          scoped_lock lock(shared_data_mutex_);
          event_que_.push_back(ButtonEvent(ButtonEvent::CHKPNT_PRESS));
          chkpnt_db_=dt;
        }
        else
          chkpnt_db_=dt; //start counter over from 0
      }

      //NEXT_PRESS
      if (data&0x4) //NEXT_PRESS is currently down
      {
        if (next_db_==0.0 || next_db_>0.5)
        {
          scoped_lock lock(shared_data_mutex_);
          event_que_.push_back(ButtonEvent(ButtonEvent::NEXT_PRESS));
          next_db_=dt;
        }
        else
          next_db_=dt; //start counter over from 0
      }

      //UNDO_PRESS
      if (data&0x2)
      {
        if (undo_db_==0.0 || undo_db_>2.0)
        {
          scoped_lock lock(shared_data_mutex_);
          event_que_.push_back(ButtonEvent(ButtonEvent::UNDO_PRESS));
          undo_db_=dt;
        }
        else
          undo_db_=dt; //start counter over from 0
      }

      //PANIC_ENGAGE, PANIC_RELEASE
      if (panic_flipped_)
      {
        if (!(data&0x1)) //was released since it was last checked
        {
          scoped_lock lock(shared_data_mutex_);
          event_que_.push_back(ButtonEvent(ButtonEvent::PANIC_RELEASE));
          panic_flipped_=false;
        }
      }
      else
      {
        if (data&0x1)
        {
          scoped_lock lock(shared_data_mutex_);
          event_que_.push_back(ButtonEvent(ButtonEvent::PANIC_ENGAGE));
          panic_flipped_=true;
        }
      }

      update_wss();

      daq_bandwidth_limiter-=0.1;
    }

    boost::thread::yield();
  }
}
