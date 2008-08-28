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
  //gps_thread_(boost::bind(&GPSThread::run,&gps_))

  
  lcd_(&kill_flag_),
  lcd_thread_(boost::bind(&DAQLCDThread::run,&lcd_))
{



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
  
  PrettyTime curtime;


  double test=0.0;
  int x=0;
  int y=0;


  lcd_.write_string(LCDString(0,0,"[dir         ][time]"));
  lcd_.write_string(LCDString(0,1,"[>dir              ]"));
  lcd_.write_string(LCDString(0,2,"[dir        ][avg/c]"));
  lcd_.write_string(LCDString(0,3,"[di"));
  lcd_.write_string(LCDString(7,4,"r       ][#][spd]"));

  //lcd_.write_string(LCDString(104,0,"X"));


  while(true)
  {
    prev_dt=dt; //calc dt of this loop
    QueryPerformanceCounter(&this_time_);
    dt = double(this_time_.QuadPart-prev_time_.QuadPart) * freq_inv_;
    prev_time_=this_time_;

    if (dt<0.0) //bug on multicore amd desktop where dt is sometimes reported as negative
      dt=0.0;

    if (GetAsyncKeyState(VK_ESCAPE))
      break;

/*
    test+=dt;

    if (test>0.25)
    {
      lcd_.write_string(LCDString(x,0,"X"));

      x++;
      if (x==128)
        x=0;

      test=0.0;
    }*/

    //curtime+=dt;

    //ostringstream out;
    //out<<curtime;

    //cout<<curtime<<" ";

    //lcd_.write_string(LCDString(0,0,out.str()));

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

//update rallyetime state
//-------------------------------------------------


//process output
//-------------------------------------------------



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