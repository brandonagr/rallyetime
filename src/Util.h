//define various utility classes used in RallyeTime
// PrettyTime, Timer, Parameter loading

#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>

std::vector<std::string> tokenize( std::string& in, std::string seps);
std::vector<std::string> tokenize_skip( std::string& in, std::string seps); 

template<typename T>
inline T convertTo(const std::string& s)
{
  if (s.length()==0)
    return T(0);

  T x;
  std::istringstream i(s);
  if (!(i >> x))
    throw std::exception(s.c_str());
  return x;
} 


//========================================================
struct PrettyTime
{
  int hour_, min_;
  double sec_;

  PrettyTime()
  {
    hour_=min_=0;
    sec_=0.0;
  }
  PrettyTime(std::string setstring, bool hascolon=true)
  {
    set(setstring,hascolon);
  }
  PrettyTime(int hour, int min, double sec)
  {
    set(hour, min, sec);    
  }
  PrettyTime(double seconds)
  {
    set(0,0,seconds);
  }

  //intialize the values to this string
  void set(std::string setstring, bool hascolon=true)
  {
    if (setstring.size()<8)
    {
      std::cout<<"Invalide string input '"<<setstring<<"'"<<std::endl;
      set(0,0,0);
      return;
    }
    std::string::size_type pos1,pos2;

    if (hascolon)
      {pos1=3;pos2=6;}
    else
      {pos1=2;pos2=4;}

    while(setstring.at(0)<'0' || setstring.at(0)>'9')//remove leading non numerals
      setstring=setstring.substr(1);

    std::stringstream conv;
    conv<<setstring.substr(0,2);
    conv>>hour_;
    
    conv.clear();
    conv<<setstring.substr(pos1,2);
    conv>>min_;

    conv.clear();
    conv<<setstring.substr(pos2);
    conv>>sec_;

    validate();
  }
  void set(int hour, int min, double sec)
  {
    hour_=hour;
    min_=min;
    sec_=sec;

    validate();
  }

  //validate all of the numbers(0-59 range)
  void validate()
  {

    while (sec_>59.0){sec_-=60.0;min_+=1;}
    while (min_>59){min_-=60;hour_+=1;}
    while (sec_<0.0){sec_+=60.0;min_-=1;}
    while (min_<0){min_+=60;hour_-=1;}  
  }

  std::string get_string()
  {
    std::ostringstream output;    
    output.fill('0');
    output.flags(std::ios_base::right);

    if (hour_>=0)
    {
      output<<" ";
      output.width(2);
      output<<hour_<<":";
      output.width(2);
      output<<min_<<":";
      output.width(2);
      output<<(int)sec_<<".";
      int decimal=(int)((sec_-(double)((int)sec_))*100.0f);
      output.width(2);
      output<<decimal;
    }
    else
    {
      output<<"-";
      output.width(2);
      output<<abs(hour_+1)<<":";
      output.width(2);

      int min=60-min_-1;
      
      double sec;
      if (sec_!=0.0)
        sec=60.0-sec_;
      else
      {
        sec=0.0;
        min+=1;
      }
    
      output<<min<<":"; 
      output.width(2);


      output<<(int)sec<<".";
      int decimal=(int)((sec-(double)((int)sec))*100.0f);
      output.width(2);
      output<<decimal;
    }

    return output.str();
  }

  double get_seconds()
  {
    return double(hour_*3600+min_*60)+sec_;
  }

  PrettyTime operator+(double seconds)
  {
    return PrettyTime(hour_,
                      min_,
                      sec_+seconds);
  }
  PrettyTime operator+(PrettyTime& otime)
  {
    return PrettyTime(hour_+otime.hour_,
                      min_+otime.min_,
                      sec_+otime.sec_);
  }
  PrettyTime operator-(PrettyTime& otime)
  {
    return PrettyTime(hour_-otime.hour_,
                      min_-otime.min_,
                      sec_-otime.sec_);
  }
  void operator+=(PrettyTime& otime)
  {
    set(hour_+otime.hour_,
        min_+otime.min_,
        sec_+otime.sec_);
  }
  void operator+=(double seconds)
  {
    set(hour_,
        min_,
        sec_+seconds);
  }
  void operator-=(PrettyTime& otime)
  {
    set(hour_-otime.hour_,
        min_-otime.min_,
        sec_-otime.sec_);
  }


  void output_internal()
  {
    std::cout<<"internal= "<<hour_<<" "<<min_<<" "<<sec_<<std::endl;
    std::cout<<"external= "<<get_string()<<std::endl;
  }
};

std::ostream& operator<<(std::ostream& os, PrettyTime& time);

//=================================================================================
class CountdownTimer
{
private:
  PrettyTime goal_;
  
  PrettyTime current_real_time_;
  PrettyTime current_dif_;
    
  bool active_;
  bool is_synced_;

public:
  CountdownTimer();

  void set_offset(PrettyTime& clock_time);
  void set_goal(PrettyTime& clock_time);
  void turn_off();
  PrettyTime& get_dif();
  PrettyTime& get_goal();

  void update(PrettyTime& real_time);      

  bool is_active(){return active_;}
};


// Loads parameter file
/*:Scene: Name

variable value
...
variable value

:Scene: Name

variable value
...


(where variable name contains no spaces)
*/
//==========================================================
class Params
{
protected:
  std::map<std::string, std::string> params_;

  friend class ParamFileReader;

public:
  Params(std::string filename);

  template <typename T>
  inline T get(const std::string name)
  {
    T val=0;

    if (params_.find(name)==params_.end())
      throw std::exception();//"Unable to find value in Params.get");

    std::stringstream ss;
    ss<<params_[name];
    ss>>val;

    return val;
  }

  template <typename T>
  inline T get_safe(const std::string name, T default_val)
  {
    if (params_.find(name)==params_.end())
      return default_val;

    T val=0;

    std::stringstream ss;
    ss<<params_[name];
    ss>>val;

    return val;
  }
  bool exists(const std::string name)
  {
    return (params_.find(name)!=params_.end());
  }
  void reset(std::string name, std::string val)
  {
    if (exists(name))
      params_.erase(params_.find(name));

    params_[name]=val;
  }
  void add(std::string name, std::string val)
  {
    params_[name]=val;
  }
};
template <>
inline std::string Params::get(const std::string name)
{
if (params_.find(name)==params_.end())
  throw std::exception();

return params_[name];
}
template <>
inline std::string Params::get_safe(const std::string name, std::string val)
{
  if (!exists(name))
    return val;

  return params_[name];
}


#endif
