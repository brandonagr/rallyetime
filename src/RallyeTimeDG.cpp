#include "RallyeTimeDG.h"

#include <iostream>
using namespace std;


//-----------------------------------------------------------
RallyeTimeDG::RallyeTimeDG(const std::string& config_file_location)
 :params_(config_file_location),
  kill_flag_(false)

  //gps_(params_, &kill_flag_),
  //gps_thread_(boost::bind(&GPSThread::run,&gps_))
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
    prev_dt=dt; //calc dt of this loop
    QueryPerformanceCounter(&this_time_);
    dt = double(this_time_.QuadPart-prev_time_.QuadPart) * freq_inv_;
    prev_time_=this_time_;

    if (dt<0.0) //bug on multicore amd desktop where dt is sometimes reported as negative
      dt=0.0;
    

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


//process
//-------------------------------------------------

    {
      IO_LOCK;
      cout<<"dt: "<<dt<<endl;
    }
    if (dt<0.1) //perform loop timing to regulate how fast it goes
    {
      int t=91-int(((dt+prev_dt)/2.0)*1000.0);

      if (t>1)
        Sleep(t); //stall some time, avg previous two frame times in order to prevent oscillating between .2 and .8 or similiar
    }
  }
}