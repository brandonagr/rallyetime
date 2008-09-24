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
    if (parts[i].compare("CHANGE")==0)
      parts[i]="";
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
RallyeDir RallyeDirections::get_dir_offset(int offset)
{
  int index=current_dir_+offset;

  if (index<0)
    return RallyeDir("",0);
  if (index>=(int)rallye_.size())
    return RallyeDir("",0);

  return rallye_[index];
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

//----------------------------------------------------------------
// 
RallyeState::RallyeState(Params params, LogManager* log)
:log_(log),
 rallye_dirs_(params.get<std::string>("RallyeDirFile")),
 leg_(0),
 section_numb_(0),
 rallye_inprogress_(false),
 distance_freeze_(false),
 timer_has_goal_(false)
{
}

//----------------------------------------------------------------
// 
void RallyeState::update(double dt, double dist)
{  
  realclock_+=dt;  
  timer_.update(realclock_); 


  if (distance_freeze_)
    dist=0;

  if (rallye_inprogress_)
  {
    leg_.update_leg(realclock_, dist);
    sectiondistance_=legstart_distance_+leg_.distance_so_far_;
  }
    
}

//----------------------------------------------------------------
// 
void RallyeState::fill_screen_active(LCDScreen& screen)
{
  if (!rallye_inprogress_)
  {
    screen.set_state_flag('S');

    screen.set_time(sectiontime_off_); //after rallye is switched to inactive, leg time is added into sectiontime_off_

    if (timer_.is_active())
    {
      screen.set_state_flag('C');
      screen.set_time(timer_.get_dif());
    }
  }
  else
  {
    if (distance_freeze_)
      screen.set_state_flag('F');
    else
      screen.set_state_flag(' ');

    screen.set_time((sectiontime_off_+(leg_.actual_time()-leg_.expected_time())));
  }  
}
void RallyeState::fill_screen_full(LCDScreen& screen)
{
  vector<string> dir_list;

  for(int i=-1; i<3; i++)
  {
    RallyeDir dir=rallye_dirs_.get_dir_offset(i);
    ostringstream out;    

    if (dir.cast_>0)
      out<<dir.cast_<<" "<<dir.dir_;
    else
      out<<dir.dir_;

    dir_list.push_back(out.str());
  }
  
  screen.set_dirs(dir_list);
  screen.set_cast((int)leg_.leg_cast_);
  screen.set_trunc_time(trunc_err_est_);
}
void RallyeState::speak_dirs(CSpeech &voice)
{
  RallyeDir curdir=rallye_dirs_.get_dir_offset(0);
  RallyeDir nextdir=rallye_dirs_.get_dir_offset(1);


  ostringstream out;

  if (curdir.cast_>0)
    out<<curdir.cast_<<" "<<curdir.dir_;
  else
    out<<curdir.dir_;

  out<<", then, ";

  if (nextdir.cast_>0)
    out<<nextdir.cast_<<" "<<nextdir.dir_;
  else
    out<<nextdir.dir_;

  voice.speak(out.str().c_str());
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
    realclock_=input;    
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
    log_->log_event(string("FREEZE DISTANCE EVENT"), LogManager::LOG);
  else
	log_->log_event(string("UNFREEZE DISTANCE EVENT"), LogManager::LOG);
}

//----------------------------------------------------------------
// 
void RallyeState::transition_to_stopped()
{
  calc_end_leg_stats();  

  timer_has_goal_=false;
  
  PrettyTime adjusted_time=section_total_time_+section_initial_off_;

  ostringstream output;  
  output<<endl<<"END SECTION: \tNumb- "<<section_numb_
	                         <<"\tAdj Time- "<<adjusted_time.get_string()<<"\t"<<adjusted_time.get_seconds()
                           <<"\tActual Time - "<<section_total_time_.get_string()<<"\t"<<section_total_time_.get_seconds()
				                   <<"\tDistance- "<<sectiondistance_/5280.0				       
	                         <<"\tDiff- "<<sectiontime_off_.get_string()<<"\t"<<sectiontime_off_.get_seconds()<<endl;
  output<<"ARRIVED TIME: \tNumb- "<<section_numb_<<"\tActual- "<<realclock_<<endl;
  log_->log_event(output.str(),LogManager::LOG);


  output.str("");
  output<<section_numb_<<"\t"<<realclock_;
  log_->log_event(output.str(),LogManager::TIMESHEET);  
}

//----------------------------------------------------------------
// 
void RallyeState::transition_to_active()
{
  //reset all the important variables
  sectiontime_off_.set(0,0,0);    
  section_total_time_.set(0,0,0);
  section_initial_off_.set(0,0,0);
  trunc_err_est_.set(0,0,0);

  sectiondistance_=0.0;  
  legstart_distance_=0.0;  
  distance_freeze_=false;

  leg_=RallyeLeg(rallye_dirs_.get_current_cast());
  leg_.update_leg(realclock_,0.0); //intialize the clock

  section_numb_++;

  if (timer_.is_active())
  {
    section_initial_off_=timer_.get_dif();
    sectiontime_off_=timer_.get_dif();

    timer_.turn_off();
  }


  ostringstream output;  
  output<<endl<<"BEGIN SECTION: \tNumb- "<<section_numb_
	                           <<"\tTimer- "<<sectiontime_off_.get_string()<<"\t"<<sectiontime_off_.get_seconds()<<endl;		
  if (timer_has_goal_)
    output<<"DEPART TIME: \tNumb- "<<section_numb_<<"\tActual- "<<realclock_<<"\tTimesheet- "<<timer_.get_goal()<<"\tDif- "<<sectiontime_off_<<endl;
  else
    output<<"DEPART TIME: \tNumb- "<<section_numb_<<"\tActual- "<<realclock_<<"\tTimesheet- "<<realclock_<<endl;

  output<<"START LEG: \tNumb- "<<rallye_dirs_.get_current_dir_numb()
	             <<"\tCast- "<<rallye_dirs_.get_current_cast()<<endl;

  log_->log_event(output.str(),LogManager::LOG);
  
  
  output.str("");  
  if (section_numb_==1)
    output<<section_numb_<<" \t";
  if (timer_has_goal_)
    output<<"\t"<<timer_.get_goal()<<endl;    
  else
    output<<"\t"<<realclock_<<endl;    
  log_->log_event(output.str(),LogManager::TIMESHEET);  
}

//----------------------------------------------------------------
// 
void RallyeState::goto_nextleg()
{
  calc_end_leg_stats();
  legstart_distance_+=leg_.distance_so_far_;

  leg_=RallyeLeg(rallye_dirs_.get_current_cast());
  leg_.update_leg(realclock_,0.0); //intialize the clock
  
  ostringstream output;
  output<<"START LEG: \tNumb- "<<rallye_dirs_.get_current_dir_numb()
	                 <<"\tCast- "<<rallye_dirs_.get_current_cast()<<endl;
  log_->log_event(output.str(),LogManager::LOG);
}

//----------------------------------------------------------------
// 
void RallyeState::calc_end_leg_stats()
{
  PrettyTime timedif=leg_.actual_time()-leg_.expected_time();

  //legstart_distance_ to sectiondistance_

  double legstart_distance_trnc=(double)(floor((legstart_distance_/5280.0)*10)/10);
  double sectiondistance_trnc=(double)(floor((sectiondistance_/5280.0)*10)/10);


  PrettyTime time_trunc=((sectiondistance_trnc-legstart_distance_trnc)/leg_.leg_cast_) * 3600; //make sure time is in seconds
  trunc_err_est_+=time_trunc-leg_.expected_time(); //amount of error from


  sectiontime_off_+=timedif;  
  section_total_time_+=leg_.actual_time();


  ostringstream output;
  output<<"END LEG: \tCast- "<<leg_.leg_cast_
			    <<"\tTime- "<<leg_.actual_time().get_string()<<"\t"<<leg_.actual_time().get_seconds()
			    <<"\tDistance- "<<leg_.distance_so_far_/5280.0
			    <<"\tIdeal- "<<leg_.expected_time().get_string()<<"\t"<<leg_.expected_time().get_seconds()
			    <<"\tOLDDiff- "<<timedif.get_string()<<"\t"<<timedif.get_seconds()
				<<"\tTruncTime- "<<time_trunc.get_string()<<"\t"<<time_trunc.get_seconds()<<endl;
  log_->log_event(output.str(),LogManager::LOG);
}