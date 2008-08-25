#include "GPS.h"

#include <iostream>
using namespace std;

//-----------------------------------------------------------
GPSThread::GPSThread(Params& params, bool* kill_flag)
 :kill_flag_(kill_flag)
{
  cout<<"made gps thread!"<<endl;
}
GPSThread::~GPSThread()
{
  cout<<"killing gps thread!"<<endl;
}



//-----------------------------------------------------------
void GPSThread::run()
{
  while(!(*kill_flag_))
  {
    boost::thread::yield();
  }

  cout<<"went outside scope"<<endl;
}

