#include "RallyeData.h"
#include <fstream>
using namespace std;


//================================================================

//----------------------------------------------------------------
// 
RallyeLeg::RallyeLeg(int leg_cast)
  :valid_(false),
   leg_cast_((double)leg_cast),
   starttime_(),
   lastrealtime_(),
   distance_so_far_(0.0),
   instant_speed_(0.0),
   leg_speed_(0.0),
   leg_time_()
{
  
}

//----------------------------------------------------------------
// 
void RallyeLeg::update_leg(PrettyTime& realtime, double distance)
{
  if (!valid_) //this is the first update in this leg
  {
    starttime_=realtime;
    valid_=true;
  }
  
  lastrealtime_=realtime;
  distance_so_far_+=distance;

  leg_time_=lastrealtime_-starttime_;
  if (leg_time_.get_seconds()==0.0)
    leg_speed_=0.0;
  else
    leg_speed_=(distance_so_far_/leg_time_.get_seconds()) * 0.68181818181818181818181818181818; //convert feet/second to miles/hour
}

//----------------------------------------------------------------
// 
PrettyTime RallyeLeg::expected_time()
{
  if (!valid_)
    return PrettyTime();

  //use cast and distance to return expected time in seconds
  //(feet * hour / mile) * 1mile/5280feet * 3600seconds/hour
  return PrettyTime((distance_so_far_/leg_cast_) * 0.68181818181818181818181818181818);
}
PrettyTime RallyeLeg::actual_time()
{
  if (!valid_)
    return PrettyTime();

  return lastrealtime_-starttime_;
}




//================================================================

//----------------------------------------------------------------
// 
RallyeDir::RallyeDir(string line)
{
  vector<string> parts(tokenize_skip(line," \t"));

  for(int i=0; i<(int)parts.size(); i++) //condense redundant directions
  {
    if (parts[i].compare("SRIP")==0)
      parts[i]="";
    if (parts[i].compare("SOL")==0)
      parts[i]="SL";
    if (parts[i].compare("SOR")==0)
      parts[i]="SR";
  }

  if (parts.back().size()<=2 && parts.size()>1) //assume last thing is a CAST if it's less than two digits
  {
    dir_=parts[0];
    for(int i=1; i<(int)parts.size()-1; i++)
    {
      dir_.append(" ");
      dir_.append(parts[i]);
    }

    stringstream conv;
    conv<<parts.back();
    conv>>cast_;
  }
  else
  {
    cast_=-1;

    dir_=parts[0];
    for(int i=1; i<(int)parts.size(); i++)
    {
      dir_.append(" ");
      dir_.append(parts[i]);
    }
  }
}
RallyeDir::RallyeDir(string line, int cast) //for a direction with a cast change
  :dir_(line),
   cast_(cast)
{ }

//----------------------------------------------------------------
// 
string RallyeDir::get_string()
{
  string line=dir_;

  if (cast_>0)
  {
    ostringstream out;

    out<<cast_<<" "<<dir_;
    line=out.str();
  }

  return line;
}
ostream& operator<<(ostream& os, RallyeDir& dir)
{
  os<<dir.dir_;
  if (dir.cast_>0)
    os<<"          "<<dir.cast_;

  return os;
}


//================================================================

//----------------------------------------------------------------
// 
RallyeDirections::RallyeDirections(string rallye_filename)
:current_dir_(0),
 last_cast_change_(0)
{
  rallye_=RallyeDirectionsFileReader(rallye_filename);
  numb_dirs_=(int)rallye_.size();

  if (rallye_filename!="./data/current/rallye_directions.txt") //save for future loads
    RallyeDirectionsFileWriter("./data/current/rallye_directions.txt",rallye_);  
}

//----------------------------------------------------------------
// 
bool RallyeDirections::advance_direction()
{  
  current_dir_++;
  
  if (current_dir_==numb_dirs_) //ran out of directions, can't go past end
  {
    current_dir_--;
    return false;
  }

  if (rallye_[current_dir_-1].cast_>0) //if completed a section with a cast change
  {
    last_cast_change_=rallye_[current_dir_-1].cast_;
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------
// 
RallyeDir& RallyeDirections::get_current_dir()
{
  return rallye_[current_dir_];
}
int RallyeDirections::get_current_cast()
{
  return last_cast_change_;
}
int RallyeDirections::get_current_dir_numb()
{
	return current_dir_;
}
int RallyeDirections::get_total_dirs()
{
  return numb_dirs_;
}

//----------------------------------------------------------------
// 
std::vector<RallyeDir> RallyeDirections::RallyeDirectionsFileReader(std::string filename)
{
  std::vector<RallyeDir> r;

  ifstream inputdata(filename.c_str());

  if (!inputdata.is_open())
    throw exception("Unable to open rallye directions!");

  string line;
  while(!inputdata.eof())
  {
    getline(inputdata,line);
    
    if (line.size()==0) //skip blank lines
      continue;

    r.push_back(RallyeDir(line));
  }
  inputdata.close();

  if (r[0].cast_==0)
  {
    cout<<"The first direction must have a cast!"<<endl;
    throw(std::exception("The first direction must have a cast!"));
  }

  return r;
}

//----------------------------------------------------------------
// 
void RallyeDirections::RallyeDirectionsFileWriter(std::string filename, std::vector<RallyeDir> rallye)
{
  ofstream outfile(filename.c_str(),ios::trunc); //deletes what was already there

  for(int i=0; i<(int)rallye.size(); i++)
    outfile<<rallye[i]<<endl;

  outfile.close();
}

//================================================================
/*
//----------------------------------------------------------------
// 
RallyeState::RallyeState(Params params)
:rallye_dirs_(params.get<std::string>("RallyeFile")),
 leg_(0),
 section_numb_(0),
 rallye_inprogress_(false),
 distance_freeze_(false),
 timer_has_goal_(false)
{
}

//----------------------------------------------------------------
// 
void RallyeState::update_time(double dt)
{
  realclock_+=dt;  
  window_data_.gps_ttl_+=dt;

  timer_.update(realclock_); 
  if (timer_.is_active())
  {        
    window_data_.timer_currenttime_value_=timer_.get_dif().get_string();
  }
  window_data_.cur_time_value_=realclock_.get_string();
}

//----------------------------------------------------------------
// 
void RallyeState::update_pos(double distance, GPSData& gpspos)
{  
  realclock_=gpspos.time_+gps_time_offset_;
  
  window_data_.gps_ttl_=0.0;

  if (distance_freeze_)
    distance=0;

  if (rallye_inprogress_)
  {    
    leg_.UpdateLeg(realclock_, distance);
    
    sectiondistance_=(legstart_distance_+leg_.distance_so_far_);    

    //strings to update
    ostringstream output;output.precision(2);        
    
    output<<std::fixed<<gpspos.speed_;
    window_data_.instant_speed_value_=output.str();output.str("");    

    output<<leg_.leg_speed_;
    window_data_.avg_speed_value_=output.str();output.str("");

    output<<leg_.leg_cast_;
    window_data_.cast_value_=output.str();output.str("");

    output<<leg_.distance_so_far_ * 1.0/5280.0; //convert feet to miles
    window_data_.leg_dist_value_=output.str();output.str("");     

    //output<<section_numb_;
    //window_data_.section_numb_value_=output.str();output.str("");

    output<<sectiondistance_ * 1.0/5280.0; //convert feet to miles
    window_data_.section_dist_value_=output.str();output.str("");

    double gain_rate=(leg_.leg_cast_-gpspos.speed_)/leg_.leg_cast_;
    output<<gain_rate;
    window_data_.time_gain_rate_value_=output.str();output.str("");

    //window_data_.leg_time_value_=(realclock_-leg_.starttime_).get_string();

    //window_data_.est_leg_result_value_=(leg_.GetActualTime()-leg_.GetExpectedTime()).get_string();    

	  window_data_.est_section_result_value_=(sectiontime_off_+(leg_.GetActualTime()-leg_.GetExpectedTime())).get_string();                     

    window_data_.current_direction_=rallye_dirs_.get_current_dir_numb();

    window_data_.gps_fix_=true;
    window_data_.last_pos_x=gpspos.state_x_;
    window_data_.last_pos_y=gpspos.state_y_;
    window_data_.last_heading=gpspos.bearing_;
  }
  
  if (rallye_inprogress_)
    window_data_.clock_status_value_="Active";
  else
    window_data_.clock_status_value_="Stopped";

  if (distance_freeze_)
    window_data_.clock_status_value_="FROZEN";
}

//----------------------------------------------------------------
// 
void RallyeState::hit_timer_go(PrettyTime target)  
{
  if (!rallye_inprogress_)
  {
    timer_.set_goal(target);
    timer_has_goal_=true;
  }
}

//----------------------------------------------------------------
// 
void RallyeState::hit_timer_sync(PrettyTime input)
{
  if (!rallye_inprogress_)
    gps_time_offset_=input-realclock_;
}

//----------------------------------------------------------------
// 
void RallyeState::hit_next()
{
  if (!rallye_inprogress_) //need to hit resume to start back up
  {
    rallye_dirs_.advance_direction();

    transition_to_active();
    rallye_inprogress_=true;    
  }
  else
  {
    if (rallye_dirs_.advance_direction()) //new cast!
      goto_nextleg();
  }
}

//----------------------------------------------------------------
// 
void RallyeState::hit_chkpnt()
{
  if (rallye_inprogress_)
  {
    transition_to_stopped();
    rallye_inprogress_=false;
  }
  else
  {
    transition_to_active();
    rallye_inprogress_=true;      
  }
}

//----------------------------------------------------------------
// 
void RallyeState::hit_freeze()
{
  distance_freeze_=!distance_freeze_;

  if (distance_freeze_)
  {
    ofstream output(LOG_FILE, ofstream::app);
    output<<"FREEZE DISTANCE EVENT"<<endl;
    output.close();
  }
}

//----------------------------------------------------------------
// 
void RallyeState::transition_to_stopped()
{
  calc_end_leg_stats();  

  timer_has_goal_=false;

  ofstream output(LOG_FILE, ofstream::app);
  PrettyTime adjusted_time=section_total_time_+section_initial_off_;
  output<<endl<<"END SECTION: \tNumb- "<<section_numb_
	                         <<"\tAdj Time- "<<adjusted_time.get_string()<<"\t"<<adjusted_time.get_seconds()
                           <<"\tActual Time - "<<section_total_time_.get_string()<<"\t"<<section_total_time_.get_seconds()
				                   <<"\tDistance- "<<sectiondistance_/5280.0				       
	                         <<"\tDiff- "<<sectiontime_off_.get_string()<<"\t"<<sectiontime_off_.get_seconds()<<endl;    
  output<<"ARRIVED TIME: \tNumb- "<<section_numb_<<"\tActual- "<<realclock_<<endl;
  output.close();

  ofstream otimesheet(TIMESHEET_FILE, ofstream::app);
  otimesheet<<section_numb_<<"\t"<<realclock_;    
  otimesheet.close();
}

//----------------------------------------------------------------
// 
void RallyeState::transition_to_active()
{
  //reset all the important variables
  legtime_off_.set(0,0,0);
  sectiontime_off_.set(0,0,0);    
  section_total_time_.set(0,0,0);
  sectiondistance_=0.0;  
  legstart_distance_=0.0;  
  distance_freeze_=false;

  leg_=RallyeLeg(rallye_dirs_.get_current_cast());
  leg_.UpdateLeg(realclock_,0.0); //intialize the clock

  window_data_.last_leg_off_value_=legtime_off_.get_string();

  section_numb_++;

  section_initial_off_=PrettyTime(0.0);
  if (timer_.is_active())
  {
    section_initial_off_=timer_.get_dif();
    sectiontime_off_=timer_.get_dif();

    timer_.turn_off();
    window_data_.timer_currenttime_value_=timer_.get_dif().get_string();
  }


  ofstream output(LOG_FILE, ofstream::app);
  output<<endl<<"BEGIN SECTION: \tNumb- "<<section_numb_
	                           <<"\tTimer- "<<sectiontime_off_.get_string()<<"\t"<<sectiontime_off_.get_seconds()<<endl;		
  if (timer_has_goal_)
    output<<"DEPART TIME: \tNumb- "<<section_numb_<<"\tActual- "<<realclock_<<"\tTimesheet- "<<timer_.get_goal()<<"\tDif- "<<sectiontime_off_<<endl;
  else
    output<<"DEPART TIME: \tNumb- "<<section_numb_<<"\tActual- "<<realclock_<<"\tTimesheet- "<<realclock_<<endl;

  output<<"START LEG: \tNumb- "<<rallye_dirs_.get_current_dir_numb()
	             <<"\tCast- "<<rallye_dirs_.get_current_cast()<<endl;
  output.close();
      
  ofstream otimesheet(TIMESHEET_FILE, ofstream::app);
  if (section_numb_==1)
    otimesheet<<section_numb_<<" \t";
  if (timer_has_goal_)
    otimesheet<<"\t"<<timer_.get_goal()<<endl;    
  else
    otimesheet<<"\t"<<realclock_<<endl;    
  otimesheet.close();

}

//----------------------------------------------------------------
// 
void RallyeState::goto_nextleg()
{
  calc_end_leg_stats();
  legstart_distance_+=leg_.distance_so_far_;

  leg_=RallyeLeg(rallye_dirs_.get_current_cast());
  leg_.UpdateLeg(realclock_,0.0); //intialize the clock This line was missing from here before, causing it to lose the time from the creation to the first update

  ofstream output(LOG_FILE, ofstream::app);
  output<<"START LEG: \tNumb- "<<rallye_dirs_.get_current_dir_numb()
	                 <<"\tCast- "<<rallye_dirs_.get_current_cast()<<endl;
  output.close();
}

//----------------------------------------------------------------
// 
void RallyeState::calc_end_leg_stats()
{
  PrettyTime timedif=leg_.GetActualTime()-leg_.GetExpectedTime();

  legtime_off_=timedif;
  sectiontime_off_+=timedif;
  section_total_time_+=leg_.GetActualTime();

  window_data_.last_leg_off_value_=legtime_off_.get_string();

  ofstream output(LOG_FILE, ofstream::app);
  output<<"END LEG: \tCast- "<<leg_.leg_cast_
			    <<"\tTime- "<<leg_.GetActualTime().get_string()<<"\t"<<leg_.GetActualTime().get_seconds()
			    <<"\tDistance- "<<leg_.distance_so_far_/5280.0
			    <<"\tIdeal- "<<leg_.GetExpectedTime().get_string()<<"\t"<<leg_.GetExpectedTime().get_seconds()
			    <<"\tDiff- "<<timedif.get_string()<<"\t"<<timedif.get_seconds()<<endl;
  output.close();
}

*/