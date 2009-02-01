#include "Util.h"

#include <string>
#include <vector>
#include <fstream>
using namespace std;


//=========================================================================
std::ostream& operator<<(std::ostream& os, PrettyTime& time)
{
  os<<time.get_string();

  return os;
}

vector<string> tokenize( string& in, string seps)
{
  unsigned int count = 0;
  vector<string> toks;
  string::size_type b = 0;
  while( (b<in.length()) )// && (string::npos != (b=in.find_first_not_of(seps, b))) ) 
  { 
     toks.push_back( in.substr(b,in.find_first_of(seps, b)-b) );
     b += toks.back().length() + 1;      
  }
  return toks;
} 
vector<string> tokenize_skip( string& in, string seps)
{
  unsigned int count = 0;
  vector<string> toks;
  string::size_type b = 0;
  while( (b<in.length())  && (string::npos != (b=in.find_first_not_of(seps, b))) ) 
  { 
     toks.push_back( in.substr(b,in.find_first_of(seps, b)-b) );
     b += toks.back().length();      
  }
  return toks;
} 

//=========================================================================
CountdownTimer::CountdownTimer()
:active_(false),
 is_synced_(false)
{

}

void CountdownTimer::set_goal(PrettyTime& clock_time)
{
  goal_=clock_time;
  active_=true;
}

void CountdownTimer::turn_off()
{
  active_=false;
  current_dif_.set(0,0,0);
}

PrettyTime& CountdownTimer::get_dif()
{
  return current_dif_;
}
PrettyTime& CountdownTimer::get_goal()
{
  return goal_;
}

void CountdownTimer::update(PrettyTime& real_time)
{
  current_real_time_=real_time;

  if (active_)
  {
    current_dif_=current_real_time_-goal_;
  }
}


//=========================================================================
//-------------------------------------------------
// Constructor
//-------------------------------------------------
Params::Params(std::string filename)
{
  std::ifstream input(filename.c_str());

  if (!input.is_open())
    throw exception("Unable to open Param file!");

  string line,key,value;

  while(getline(input,line))
  {
    if (line.size()==0 || line.substr(0,1)=="#")
      continue; 

    key=line.substr(0,line.find(" "));
    value=line.substr(line.find(" ")+1);

    params_[key]=value;
  }
}
