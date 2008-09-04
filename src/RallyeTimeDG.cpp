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
  /*
  PrettyTime curtime;
  double time_since_update=0.0;


  double test=0.0;
  int x=0;
  int y=0;


  lcd_.write_string(LCDString(0,0,"[dir         ][time]"));
  lcd_.write_string(LCDString(0,1,"[>dir              ]"));
  lcd_.write_string(LCDString(0,2,"[dir        ][avg/c]"));
  lcd_.write_string(LCDString(0,3,"[dir       ][#][spd]"));
*/
  screen_.clear();

  screen_.set_fullscreen_mode(true);

  PrettyTime curtime(0.0);
  GPSData last_pos;
  double dist=0.0;
  double gps_spd=0.0;


  while(true)
  {
    prev_dt=dt; 
    QueryPerformanceCounter(&this_time_); //calc dt of this loop
    dt = double(this_time_.QuadPart-prev_time_.QuadPart) * freq_inv_;
    prev_time_=this_time_;

    key_input_.update();

    if (key_input_.keydown(VK_ESCAPE)) //exit entire program
      break;


 //   if (gps_.is_gps_update())
    {
 //     GPSData cur_pos=gps_.get_gps_update();

   //   gps_spd=cur_pos.speed_;

      /*
      if (!last_pos.valid_)
        last_pos=cur_pos;

      dist+=last_pos.distance(cur_pos);

      last_pos=cur_pos;

      ostringstream out;
      out<<cur_pos;      
      log_.log_event(out.str(),LogManager::GPSLOG);
      */
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

    curtime+=dt;

    screen_.set_cur_speed(input_.get_instant_speed());

	  screen_.set_cur_avg_speed(gps_spd);

    dist+=input_.get_distance();
    screen_.set_time(PrettyTime(dist/5280.0));

//process output
//-------------------------------------------------

    screen_.update(dt); //output data to realtime screen


    /*
    {
      IO_LOCK;
      cout<<"dt: "<<dt<<endl;
    }
    */
    if (dt<0.1) //perform loop timing to regulate how fast it goes
    {
      int t=100-int(((dt+prev_dt)/2.0)*1000.0);

      if (t>1)
        Sleep(t); //stall some time, avg previous two frame times in order to prevent oscillating between .2 and .8 or similiar
    }
  }
}