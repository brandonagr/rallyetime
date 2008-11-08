#include "RallyeTimeDG.h"

#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>


//-----------------------------------------------------------
RallyeTimeDG::RallyeTimeDG(const std::string& config_file_location)
 :params_(config_file_location),
  kill_flag_(false),

#ifdef ENABLE_GPS
  gps_(params_, &kill_flag_),
  gps_thread_(boost::bind(&GPSThread::run,&gps_)),
#endif
  
  lcd_(&kill_flag_),
  lcd_thread_(boost::bind(&DAQLCDThread::run,&lcd_)),
  screen_(&lcd_),

  input_(&kill_flag_, params_),
  input_thread_(boost::bind(&DAQButtonThread::run,&input_)),
  latest_instant_speed_(0.0),

  log_(&kill_flag_),
  log_thread_(boost::bind(&LogManager::run,&log_)),

  cur_state_(RallyeTimeDG::SETUP_ONE)
{
  voice_.speak("Initializing system.");

  switch_to_setup_one();
}


//-----------------------------------------------------------
RallyeTimeDG::~RallyeTimeDG()
{
  {
    IO_LOCK;
    cout<<"Attempting to terminate threads..."<<endl;
  }

  kill_flag_=true;

#ifdef ENABLE_GPS
  gps_thread_.join();
#endif
  lcd_thread_.join();
  input_thread_.join();
  log_thread_.join();

  {
    IO_LOCK;
    cout<<"Done cleaning up threads..."<<endl;
  }
}


//-----------------------------------------------------------
// Entry point for execution of program
void RallyeTimeDG::run_till_quit()
{
  LARGE_INTEGER freq_, prev_time_, this_time_;   
  double dt=0.0,prev_dt=0.0;
  QueryPerformanceFrequency(&freq_);
  double freq_inv_=1/(double)freq_.QuadPart;
  QueryPerformanceCounter(&prev_time_);  

  while(true)
  {
    prev_dt=dt; 
    QueryPerformanceCounter(&this_time_); //calc dt of this loop
    dt = double(this_time_.QuadPart-prev_time_.QuadPart) * freq_inv_;
    prev_time_=this_time_;


    //--------------------------------------------------------
    // updates that get called in any state
    official_time_+=dt;

    key_input_.update();

    if (key_input_.keydown(VK_ESCAPE)) //exit entire program
      break;

#ifdef ENABLE_GPS
    if (gps_.is_gps_update())
    {
      last_gps_pos_=gps_.get_gps_update();

      ostringstream out;out<<last_gps_pos_;
      log_.log_event(out.str(),LogManager::GPSLOG);
    }
#endif

    latest_instant_speed_=input_.get_instant_speed();
    screen_.set_cur_speed(latest_instant_speed_);

    //--------------------------------------------------------
    // state based updates
    switch(cur_state_)
    {
    case SETUP_ONE:

      update_setup_one(dt);

      break;
    case SETUP_TWO:

      update_setup_two(dt);

      break;
    case STAGING:

      update_staging(dt);

      break;
    case RALLYE:

      rallye(dt);

      break;
    }


//process output
//-------------------------------------------------

    screen_.update(dt); //output data to realtime screen


    if (dt<0.1) //perform loop timing to regulate how fast it goes
    {
      int t=100-int(((dt+prev_dt)/2.0)*1000.0);

      if (t>1)
        Sleep(t); //stall some time, avg previous two frame times in order to prevent oscillating between .2 and .8 or similiar
    }
  }
}


//-----------------------------------------------------------
// first stage, enter official time
void RallyeTimeDG::switch_to_setup_one()
{
  cur_state_=SETUP_ONE;

  screen_.clear();
  screen_.set_fullscreen_mode(false);
}
void RallyeTimeDG::update_setup_one(double dt)
{
  static bool entering_time=false;

  if (last_gps_pos_.valid_)
    lcd_.write_string(LCDString(11,0,"GPS Good "));
  else
    lcd_.write_string(LCDString(11,0,"GPS ERROR"));
  
  if (!entering_time && key_input_.keypress(VK_RETURN))
  {
    voice_.speak("enter official time.");
    key_input_.start_time_input();

    entering_time=true;
  }

  if (entering_time)
  {
    string cur_input=key_input_.get_time();

    ostringstream out;
    out<<"OT: "<<cur_input;
    lcd_.write_string(LCDString(0,3,out.str(),10,false));

    if (key_input_.is_done()) //check if value is good, if so, move to next direction
    {
      if (cur_input.size()==6)
      {
        official_time_=PrettyTime(cur_input,false);

        out.str("");
        out<<"Set OT: "<<official_time_;
        log_.log_event(out.str(),LogManager::GPSLOG);
        
        switch_to_setup_two();
      }
      else
      {
        voice_.speak("in valid, time");
        lcd_.write_string(LCDString(0,3,"",10,false));
        entering_time=false;
      }
    }
  }
}

//-----------------------------------------------------------
// second stage, get directions
void RallyeTimeDG::switch_to_setup_two()
{
  cur_state_=SETUP_TWO;
  screen_.clear();
  lcd_.write_string(LCDString(0,0,"Awaiting dirs..."));
  voice_.speak("load directions");
}
void RallyeTimeDG::update_setup_two(double dt)
{
  if (key_input_.keydown('R') && key_input_.keydown(VK_CONTROL)) //attempt to load the directions
  {
    ifstream in(params_.get<string>("RallyeDirFile").c_str());

    if (in.is_open())
    {      
      rallye_states_.push_front(RallyeState(params_, &log_));
      rallye_states_.front().hit_timer_sync(official_time_);
      
      voice_.speak("directions loaded");
      switch_to_staging();
    }
    else
    {
      voice_.speak("unable to load directions");
      cout<<"ERROR"<<endl;
    }

    in.close();
  }
}

//-----------------------------------------------------------
// staging
void RallyeTimeDG::switch_to_staging()
{
  cur_state_=RallyeTimeDG::STAGING;

  //output initial directions
  screen_.set_fullscreen_mode(true);
  rallye_states_.front().fill_screen_active(screen_);
  rallye_states_.front().fill_screen_full(screen_);
}
void RallyeTimeDG::update_staging(double dt)
{
  static bool entering_time=false;

  if (!entering_time && key_input_.keypress(VK_RETURN))
  {
    voice_.speak("enter leave time.");
    key_input_.start_time_input();

    entering_time=true;
  }

  if (entering_time)
  {
    string cur_input=key_input_.get_time();

    ostringstream out;
    out<<"LT: "<<cur_input;
    lcd_.write_string(LCDString(0,3,out.str(),10,false));

    if (key_input_.is_done()) //check if value is good, if so, move to next direction
    {
      if (cur_input.size()==6)
      {
        PrettyTime leave_target=PrettyTime(cur_input,false);

        out.str("");
        out<<"Set LT: "<<leave_target;
        log_.log_event(out.str(),LogManager::GPSLOG);

        rallye_states_.front().hit_timer_go(leave_target);
        rallye_states_.front().fill_screen_full(screen_);
      }
      else
      {
        voice_.speak("in valid, time");
        lcd_.write_string(LCDString(0,3,"",10,false));
      }
      entering_time=false;
    }
  }

  double dist=input_.get_distance();
  for(list<RallyeState>::iterator i=rallye_states_.begin(); i!=rallye_states_.end(); i++)
    i->update(dt, dist);

  rallye_states_.front().fill_screen_active(screen_);

  if (input_.is_event())
  {
    ButtonEvent button=input_.get_event();

    switch(button.type_)
    {
    case ButtonEvent::NEXT_PRESS:
      {
        switch_to_rallye();

        save_state();
        rallye_states_.front().hit_next();
        rallye_states_.front().fill_screen_full(screen_);
        rallye_states_.front().speak_dirs(voice_);

        log_.log_event(string("HIT_NEXT"),LogManager::GPSLOG);

        //voice_.speak("next");
      }
      break;
    case ButtonEvent::CHKPNT_PRESS:
      {
        switch_to_rallye();

        save_state();
        rallye_states_.front().hit_chkpnt();
        rallye_states_.front().fill_screen_full(screen_);
        rallye_states_.front().speak_dirs(voice_);

        log_.log_event(string("HIT_CHKPNT"),LogManager::GPSLOG);

        //voice_.speak("checkpoint");
      }
      break;
    case ButtonEvent::UNDO_PRESS:
      {
        if (rallye_states_.size()>1)
        {
          rallye_states_.pop_front();
          if (rallye_states_.front().is_inprogress())
            switch_to_rallye();
          rallye_states_.front().fill_screen_full(screen_);

          log_.log_event(string("HIT_UNDO"),LogManager::GPSLOG);

          voice_.speak("undo");
        }
        else
          voice_.speak("error no more undo");
      }
      break;
    case ButtonEvent::PANIC_ENGAGE:
      {
        voice_.speak("don't push my buttons");
      }
      break;
    case ButtonEvent::PANIC_RELEASE:
      {
        voice_.speak("that's better");
      }
      break;
    }
  }
}

//-----------------------------------------------------------
// staging
void RallyeTimeDG::switch_to_rallye()
{
  cur_state_=RallyeTimeDG::RALLYE;

  //output initial directions
  screen_.set_fullscreen_mode(true);
  rallye_states_.front().fill_screen_active(screen_);
  rallye_states_.front().fill_screen_full(screen_);
}
void RallyeTimeDG::rallye(double dt)
{
  //verbal warning every 7 seconds when you are speeding 12 over the speed limit
  static double time_speeding=0.0;
  if (latest_instant_speed_>rallye_states_.front().get_cast()+17.0)
    time_speeding+=dt;
  else
    time_speeding=0.0;

  if (time_speeding>7.0) //over 12 over for more than 7 seconds
  {
    time_speeding=0.0;
    ostringstream out;
    out<<"speeding "<<(int)(latest_instant_speed_-(rallye_states_.front().get_cast()+5))<<" over";
    voice_.speak(out.str().c_str());
  }

  double dist=input_.get_distance();
  for(list<RallyeState>::iterator i=rallye_states_.begin(); i!=rallye_states_.end(); i++)
    i->update(dt, dist);

  rallye_states_.front().fill_screen_active(screen_);

  if (input_.is_event())
  {
    ButtonEvent button=input_.get_event();
	time_speeding=0.0;

    switch(button.type_)
    {
    case ButtonEvent::NEXT_PRESS:
      {
        save_state();
        rallye_states_.front().hit_next();
        rallye_states_.front().fill_screen_full(screen_);
        rallye_states_.front().speak_dirs(voice_);

        log_.log_event(string("HIT_NEXT"),LogManager::GPSLOG);

        //voice_.speak("next");
      }
      break;
    case ButtonEvent::CHKPNT_PRESS:
      {
        switch_to_staging();

        save_state();
        rallye_states_.front().hit_chkpnt();
        rallye_states_.front().fill_screen_full(screen_);
        //rallye_states_.front().speak_dirs(voice_);

        log_.log_event(string("HIT_CHKPNT"),LogManager::GPSLOG);

        voice_.speak("checkpoint");
      }
      break;
    case ButtonEvent::UNDO_PRESS:
      {
        if (rallye_states_.size()>1)
        {
          rallye_states_.pop_front();
          if (!rallye_states_.front().is_inprogress())
            switch_to_staging();
          rallye_states_.front().fill_screen_full(screen_);

          log_.log_event(string("HIT_UNDO"),LogManager::GPSLOG);
          voice_.speak("undo");
        }
        else
          voice_.speak("error no more undo");
      }
      break;
    case ButtonEvent::PANIC_ENGAGE:
      {
	    for(list<RallyeState>::iterator i=rallye_states_.begin(); i!=rallye_states_.end(); i++)
    		i->hit_freeze();

        log_.log_event(string("HIT_FREEZE (engage)"),LogManager::GPSLOG);
        voice_.speak("freeze");
      }
      break;
    case ButtonEvent::PANIC_RELEASE:
      {
		for(list<RallyeState>::iterator i=rallye_states_.begin(); i!=rallye_states_.end(); i++)
    		i->hit_freeze();

        log_.log_event(string("HIT_FREEZE (release)"),LogManager::GPSLOG);
        voice_.speak("unfreeze");
      }
      break;
    }
  }
}

