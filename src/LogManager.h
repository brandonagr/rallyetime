/* ================================================================

 Handle logging events to file and also running TTS commands

 Seperated to seperate thread because HD will be set to parked by default
  so when it occasionally writes to file there will be a significant delay
  as head unparks

Brandon Green - 08-08-24
================================================================= */
#ifndef LOGMANAGER_H_
#define LOGMANAGER_H_

#include "ThreadSupport.h"
#include <vector>


#define LOG_FILE "data/current/rallye_log.txt"
#define TIMESHEET_FILE "data/current/rallye_timesheet.txt"
#define GPSLOG_FILE "data/current/rallye_gpsdata.txt"


//----------------------------------------------------------------
// Handle logging everything
class LogManager
{
private:
  bool* kill_flag_;

  //shared data----
  boost::mutex shared_data_mutex_;
  std::vector<std::string> logfile_log_;
  std::vector<std::string> timesheet_log_;
  std::vector<std::string> gpslog_log_;
  bool flush_log_;
  //---------------

  void write_logs();

public:
  LogManager(bool* kill_flag);
  ~LogManager();

  void run();


  enum LogType {LOG, TIMESHEET, GPSLOG};

  //called by main thread, places string in que
  void log_event(std::string& str, LogType type);  
  void flush_logs();
};



#endif