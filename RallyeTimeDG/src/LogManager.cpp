#include "LogManager.h"
#include <fstream>
using namespace std;


//----------------------------------------------------------------
// 
LogManager::LogManager(bool* kill_flag)
 :kill_flag_(kill_flag),
  flush_log_(false),

  timestamp_(0.0)
{
  logfile_log_.reserve(100);
  timesheet_log_.reserve(10);
  gpslog_log_.reserve(500);
}
LogManager::~LogManager()
{
  write_logs();
}


//----------------------------------------------------------------
// 
void LogManager::log_event(std::string& str, LogType type)
{
  scoped_lock lock(shared_data_mutex_);

  ostringstream ts;
  ts<<"\t("<<timestamp_<<")";
  str.append(ts.str());

  switch(type)
  {
  case LogManager::LOG:
    logfile_log_.push_back(str);
    break;
  case LogManager::TIMESHEET:
    timesheet_log_.push_back(str);
    break;
  case LogManager::GPSLOG:
    gpslog_log_.push_back(str);
    break;
  }
}
void LogManager::flush_logs()
{
  flush_log_=true;
}


//----------------------------------------------------------------
// 
void LogManager::write_logs()
{
  std::vector<std::string> temp;

  temp.clear();
  {    
    scoped_lock lock(shared_data_mutex_);
    if (logfile_log_.size()!=0)
    {
      temp=logfile_log_;
      logfile_log_.clear();
    }
  }
  if (temp.size()!=0)
  {
    ofstream output(LOG_FILE, ofstream::app);
    for(int i=0; i<(int)temp.size(); i++)
      output<<temp[i]<<endl;
    output.close();
    temp.clear();
  }

  {
    scoped_lock lock(shared_data_mutex_);
    if (timesheet_log_.size()!=0)
    {
      temp=timesheet_log_;
      timesheet_log_.clear();
    }
  }
  if (temp.size()!=0)
  {
    ofstream output(TIMESHEET_FILE, ofstream::app);
    for(int i=0; i<(int)temp.size(); i++)
      output<<temp[i]<<endl;
    output.close();
    temp.clear();
  }

  {
    scoped_lock lock(shared_data_mutex_);
    if (gpslog_log_.size()!=0)
    {
      temp=gpslog_log_;
      gpslog_log_.clear();
    }
  }
  if (temp.size()!=0)
  {
    ofstream output(GPSLOG_FILE, ofstream::app);
    for(int i=0; i<(int)temp.size(); i++)
      output<<temp[i]<<endl;
    output.close();
    temp.clear();
  }
}


//----------------------------------------------------------------
// 
void LogManager::run()
{
    LARGE_INTEGER freq_, prev_time_, this_time_;   
  double dt=0.0;
  QueryPerformanceFrequency(&freq_);
  double freq_inv_=1/(double)freq_.QuadPart;
  QueryPerformanceCounter(&prev_time_);  

  double start=0.0;
  double time=0.0;
  

  while(!(*kill_flag_))
  {
    //get dt of the loop
    QueryPerformanceCounter(&this_time_);
    dt = double(this_time_.QuadPart-prev_time_.QuadPart) * freq_inv_;
    prev_time_=this_time_;

    time+=dt;

    {
      scoped_lock lock(shared_data_mutex_);
      timestamp_=time;      
    }

    if (time-start>300 || flush_log_) //5 min is 300 seconds, 300 000 milliseconds
    {
      write_logs();

      start=time;
      flush_log_=false;
    }

    boost::thread::yield();
  }
}