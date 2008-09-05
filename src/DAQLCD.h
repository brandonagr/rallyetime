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
  LCDString(unsigned char x, unsigned char y, std::string data, int field_size, bool justify_right=true)
    :x_(x),
     y_(y)
  {
    if ((int)data.size()>field_size) //truncate
      data_=data.substr(0,field_size);
    else
      if ((int)data.size()<field_size) //need some padding
      {
        if (justify_right)
        {
          data_=std::string(field_size-(int)data.size(),' ');
          data_.append(data);
        }
        else
        {
          data_=data;
          data_.append(std::string(field_size-(int)data.size(),' '));
        }
      }
      else
        data_=data; //already the right size
  }
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
  void clear();
};


//================================================================


const double LCDSCREEN_REFRESH_RATE=0.1; //10hz update rate


//----------------------------------------------------------------
// manage lcd screen output, exists in main thread
class LCDScreen
{
  DAQLCDThread* lcd_;

  double time_since_write_;

  char state_flag_;
  double time_;
  double avg_spd_;
  double spd_;

  bool enable_fullscreen_; 

  bool full_redraw_; //only full redraw after a set_dirs call
  int cast_;
  int dir_numb_;
  std::vector<std::string> dirs_;

 


/*
  lcd_.write_string(LCDString(0,0,"[dir         ][time]"));
  lcd_.write_string(LCDString(0,1,"[dir               ]"));
  lcd_.write_string(LCDString(0,2,"[dir        ][avg/c]"));
  lcd_.write_string(LCDString(0,3,"[dir       ][#][spd]"));

  dir line 1= 14
  dir line 2= 20
  dir line 3= 13
  dir line 4= 12

  time= [char]{+,-}###.#
  avg= ##.#/##
  #(cur dir number)= ###
  spd= >##.#
*/

public:
  LCDScreen(DAQLCDThread* lcd);


  void set_dirs(std::vector<std::string>& dirs);
  void set_cast(int cast);
  void set_dir_numb(int numb);

  void set_state_flag(char flag){state_flag_=flag;}
  void set_time(PrettyTime& time);
  void set_cur_speed(double cur_speed);
  void set_cur_avg_speed(double cur_avg_speed);

  void update(double dt); 

  void clear(){lcd_->clear();}

  void set_fullscreen_mode(bool val){enable_fullscreen_=val;}
};
#endif