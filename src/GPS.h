/* ================================================================

 Run the GPS monitor thread

Brandon Green - 08-08-24
================================================================= */
#ifndef GPS_H_
#define GPS_H_

#include "Util.h"

#include <string>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#pragma comment(lib, "boost_thread-vc80-mt-1_35.lib")

typedef boost::mutex::scoped_lock scoped_lock;


// Holds individual GPS update
//=====================================================================
struct GPSData
{
public:  
  bool valid_; //flag that says if this is a valid position

  double lat_; //both stored in degrees
  double lon_;
  double state_x_, state_y_; //stored in feet, derived from lat/long

  PrettyTime time_;
  double speed_;
  double bearing_;

  
  enum GPGGA {GGA_TIME=1,
              GGA_LATITUDE,
              GGA_LAT_DIR,
              GGA_LONGITUDE,
              GGA_LONG_DIR,
              GGA_VALID,
              GGA_NUMB_SATS,
              GGA_HDOP,
              GGA_ALTITUDE,
              GGA_ALT_UNITS,
              GGA_GEOID,
              GGA_GEOID_UNITS,
              GGA_BLANK,
              GGA_CHECKSUM};
  
  enum GPRMC {RMC_TIME=1,
              RMC_SAT_FIX_STATUS,
              RMC_LATITUDE,
              RMC_LAT_DIR,
              RMC_LONGITUDE,
              RMC_LON_DIR,
              RMC_SPEED,
              RMC_BEARING,
              RMC_DATE,
              RMC_CHECKSUM};

public:
  GPSData();
  GPSData(std::string& gpsstring);

  double distance(const GPSData& opos);
  void calc_state();

  static inline bool is_GPRMC(std::string& gpsstring)
  {
    return (gpsstring.substr(0,6)=="$GPRMC");
  }
  static inline bool is_GPGGA(std::string& gpsstring)
  {
    return (gpsstring.substr(0,6)=="$GPGGA");
  }
};
std::ostream& operator << (std::ostream& os, GPSData& pos);


// wrap locking shared data
//  ASSUME - that get gets called rapidly enough so that a GPSData update is not overwritten
//================================================================
class GPSBuffer
{
private:
  boost::mutex mutex;
  
  bool empty;
  GPSData latest;

public:
  GPSBuffer();
    
  void put(GPSData& data); //called by GPSThread when it gets an update
  GPSData get(); //called by RallyeTimeDG thread to get latest GPS update
  bool is_empty();
};


//================================================================
class GPSThread
{
private:
  bool* kill_flag_;

  GPSBuffer buffer_;


public:
  GPSThread(Params& params, bool* kill_flag);
  ~GPSThread();  

  void run();

  //called by external threads
  bool is_gps_update(){return !buffer_.is_empty();};
  GPSData get_gps_update(){return buffer_.get();};
};


#endif
