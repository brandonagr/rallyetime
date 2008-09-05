#include "RallyeTimeDG.h"

#include <iostream>
#include <sstream>
using namespace std;

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>


//-----------------------------------------------------------
RallyeTimeDG::RallyeTimeDG(const std::string& config_file_location)
 :params_(config_file_location),
  kill_flag_(false),

  //gps_(params_, &kill_flag_),
  //gps_thread_(boost::bind(&GPSThread::run,&gps_)),
  
  lcd_(&kill_flag_),
  lcd_thread_(boost::bind(&DAQLCDThread::run,&lcd_)),
  screen_(&lcd_),

  input_(&kill_flag_, params_),
  input_thread_(boost::bind(&DAQButtonThread::run,&input_)),

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

  //gps_thread_.join();
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

//    if (gps_.is_gps_update())
//      last_gps_pos_=gps_.get_gps_update();

    screen_.set_cur_speed(input_.get_instant_speed());

    //--------------------------------------------------------
    // state based updates
    switch(cur_state_)
    {
    case SETUP_ONE:

      update_setup_one(dt);

      break;
    case SETUP_TWO:

      break;
    case STAGING:

      break;
    case RALLYE:

      break;
    }

/*  //testing asynchronous input  
    if (key_input_.keypress('S'))
      key_input_.start_time_input();

    if (!key_input_.is_done())
      cout<<"Currently: '"<<key_input_.get_time()<<"'"<<endl;
      */

/*
    time_since_update+=dt;
    curtime+=dt;

    if (time_since_update>0.1)
    {
      ostringstream out;
      out<<curtime;
      //cout<<curtime<<" ";
      lcd_.write_string(LCDString(0,0,out.str()));
      log_.log_event(out.str(), LogManager::GPSLOG);

      time_since_update-=0.1;
    }
*/
//process inputs
//-------------------------------------------------
    /*
    if (gps_.is_gps_update())
    {
      GPSData update(gps_.get_gps_update());

      {
        IO_LOCK;
        cout<<"Got GPS update: "<<update<<endl;
        
      }
    }
    */

    if (input_.is_event())
    {
      ButtonEvent button=input_.get_event();

      switch(button.type_)
      {
      case ButtonEvent::NEXT_PRESS:
        {
          IO_LOCK;
          cout<<"NEXT_PRESS"<<endl;
        }
        break;
      case ButtonEvent::CHKPNT_PRESS:
        {
          IO_LOCK;
          cout<<"CHKPNT_PRESS"<<endl;
        }
        break;
      case ButtonEvent::UNDO_PRESS:
        {
          IO_LOCK;
          cout<<"UNDO_PRESS"<<endl;
        }
        break;
      case ButtonEvent::PANIC_ENGAGE:
        {
          IO_LOCK;
          cout<<"PANIC_ENGAGE"<<endl;
        }
        break;
      case ButtonEvent::PANIC_RELEASE:
        {
          IO_LOCK;
          cout<<"PANIC_RELEASE"<<endl;
        }
        break;
      }
    }

//update rallyetime state
//-------------------------------------------------

/*
    curtime+=dt;

    screen_.set_cur_speed(input_.get_instant_speed());

	  screen_.set_cur_avg_speed(gps_spd);

    dist+=input_.get_distance();
    screen_.set_time(PrettyTime(dist/5280.0));
*/


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
    lcd_.write_string(LCDString(12,0,"GPS Good"));
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
        log_.log_event(out.str(),LogManager::LOG);
        
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
  if (key_input_.keypress('R')) //attempt to load the directions
  {


  }
}