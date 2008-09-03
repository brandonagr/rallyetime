/* ================================================================

 Thread that runs the LCD display

Brandon Green - 08-08-27
================================================================= */
#ifndef DAQLCD_H_
#define DAQLCD_H_


#include "DAQ.h"
#include "ThreadSupport.h"
#include "Util.h"
#include <list>


/*
LCD Mapping, WHICH MAKES NO SENSE!

0----------------19
40---------------59
20---------------39
84--------------103
*/

//----------------------------------------------------------------
struct LCDString
{
  unsigned char x_, y_;
  std::string data_;

  LCDString(unsigned char x, unsigned char y, std::string data)
    :x_(x),
     y_(y),
     data_(data)
  {}
  LCDString()
    :x_(0),
     y_(0)
  {}
};

//----------------------------------------------------------------
class DAQLCDThread
{
private:
  bool* kill_flag_;

  //ports for lcd output
  DAQTask p0_;
  DAQTask p1_;

  //shared data----
  boost::mutex shared_data_mutex_;
  std::list<LCDString> write_que_;
  //---------------
  
  void write_data(unsigned char data, bool rs=false);
  void set_location(unsigned char x, unsigned char y);
  void internal_write_string(LCDString& string);
  

public:
  DAQLCDThread(bool* kill_flag);
  ~DAQLCDThread();

  void run();

  //called by main thread, places string in que
  void write_string(LCDString& data);  
};

//----------------------------------------------------------------
// manage lcd screen output, exists in main thread
class LCDScreen
{
  DAQLCDThread& lcd_;

/*
  lcd_.write_string(LCDString(0,0,"[dir         ][time]"));
  lcd_.write_string(LCDString(0,1,"[dir               ]"));
  lcd_.write_string(LCDString(0,2,"[dir        ][avg/c]"));
  lcd_.write_string(LCDString(0,3,"[dir       ][#][spd]"));

  dir line 1= 14
  dir line 2= 20
  dir line 3= 13
  dir line 4= 12

  time= {+,-}###.#
  avg= ##.#/##
  #(cur dir number)= ###
  spd= >##.#
*/

  //3 realtime fields
  //

public:
  LCDScreen(DAQLCDThread& lcd);


  void set_time(PrettyTime& time);
  void set_cur_speed(double cur_speed);
  void set_cur_avg_speed(double cur_avg_speed);

  void update(double dt); 
};
#endif