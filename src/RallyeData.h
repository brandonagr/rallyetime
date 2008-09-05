/* ================================================================

 Data structures to hold rallye data

Brandon Green - 08-09-02
================================================================= */
#ifndef RALLYEDATA_H_
#define RALLYEDATA_H_

#include "Util.h"
#include "DAQLCD.h"
#include "LogManager.h"
#include "Speech.h"


//================================================================
//
class RallyeLeg //A leg is the part run at a particular cast
{
public:
  PrettyTime starttime_, lastrealtime_;
  double distance_so_far_; //in feet
  bool valid_;

  double leg_cast_;

  //calculated values
  double instant_speed_;
  double leg_speed_;
  PrettyTime leg_time_;

public:
  RallyeLeg(int leg_cast);

  void update_leg(PrettyTime& realtime, double distance);

  PrettyTime expected_time();
  PrettyTime actual_time();
};


//================================================================
//
class RallyeDir
{
public:
  std::string dir_;
  int cast_;

  PrettyTime timedelay_;

  RallyeDir(std::string line); //auto extract if there is a cast change on this line
  RallyeDir(std::string line, int cast);

  std::string get_string();
  bool has_cast_change(){return (cast_>0);}
};
std::ostream& operator<<(std::ostream& os, RallyeDir& dir);


//================================================================
//
class RallyeDirections
{
private:
  std::vector<RallyeDir> rallye_;
  int current_dir_; //this is the index of the direction you are approaching
  int numb_dirs_;

  int last_cast_change_;

public:
  RallyeDirections(std::string rallye_filename);


  bool advance_direction(); //return true if there was a cast change
  RallyeDir& get_current_dir();
  RallyeDir get_dir_offset(int offset);
  int get_current_cast();
  int get_current_dir_numb();
  int get_total_dirs();

  static std::vector<RallyeDir> RallyeDirectionsFileReader(std::string filename);
  static void RallyeDirectionsFileWriter(std::string filename, std::vector<RallyeDir> rallye);
};


//================================================================
//
class RallyeState
{
private:
  LogManager* log_;

  RallyeDirections rallye_dirs_;
  RallyeLeg leg_; 

  PrettyTime realclock_; 
  
  bool rallye_inprogress_; //true if the rallye is currently going
  bool distance_freeze_;

  int section_numb_;
  PrettyTime legtime_off_;
  PrettyTime sectiontime_off_;
  PrettyTime section_total_time_;
  PrettyTime section_initial_off_;
  double sectiondistance_;
  double legstart_distance_;

  CountdownTimer timer_;
  bool timer_has_goal_;

  void transition_to_stopped();
  void transition_to_active();
  void goto_nextleg();
  void calc_end_leg_stats();


public:
  RallyeState(Params params, LogManager* log);

  void update(double dt, double dist);

  void fill_screen_active(LCDScreen& screen); //display all the actively updated variables
  void fill_screen_full(LCDScreen& screen); //load up all the directions
  void speak_dirs(CSpeech& voice);

  void hit_timer_go(PrettyTime target);
  void hit_timer_sync(PrettyTime input);

  void hit_next();
  void hit_chkpnt();
  void hit_freeze();

  bool is_inprogress(){return rallye_inprogress_;}
};


#endif