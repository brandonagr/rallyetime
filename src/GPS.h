/* ================================================================

 Run the GPS monitor thread

Brandon Green - 08-08-24
================================================================= */
#ifndef GPS_H_
#define GPS_H_

#include "Util.h"
#include "ThreadSupport.h"
#include "SerialPort.h"
#include <string>


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


//================================================================
class GPSThread
{
private:
  bool* kill_flag_;
  
  //shared data---
  boost::mutex shared_data_mutex_;  
  bool empty_;
  GPSData latest_;
  //---------------

  CSerialPort gpsport_;
  std::string get_gps_line();

public:
  GPSThread(Params& params, bool* kill_flag);
  ~GPSThread();  

  void run();

  //called by external threads---
  bool is_gps_update();
  GPSData get_gps_update();
  //-----------------------------
};


#endif
