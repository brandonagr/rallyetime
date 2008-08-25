#include "RallyeTimeDG.h"

#include <boost/bind.hpp>

#include <iostream>
using namespace std;


//-----------------------------------------------------------
RallyeTimeDG::RallyeTimeDG(const std::string& config_file_location)
 :params_(config_file_location),
  kill_flag_(false),

  gps_(params_, &kill_flag_),
  gps_thread_(new boost::thread(boost::bind(&GPSThread::run,&gps_)))
{



}


//-----------------------------------------------------------
RallyeTimeDG::~RallyeTimeDG()
{
  cout<<"Attempting to terminate threads..."<<endl;

  kill_flag_=true;
  gps_thread_->join();

  cout<<"Done cleaning up threads..."<<endl;
}


//-----------------------------------------------------------
// Entry point for execution of program
void RallyeTimeDG::run_till_quit()
{
  cout<<"yo"<<endl;
  Sleep(500);
}