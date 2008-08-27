#include "RallyeTimeDG.h"

#include <iostream>
#include <memory>
using namespace std;

#define WINDOW_LEAN_AND_MEAN
#include <windows.h>


int main()
{
  const string config_file="data/settings.txt";

  //Hack so desktop will behave the same as laptop
  HANDLE hProcess = GetCurrentProcess();
  SetProcessAffinityMask( hProcess, 1L );// use CPU 0 only

  try
  {
    cout<<"Attempting to construct RallyeTimeDG class..."<<endl;
    auto_ptr<RallyeTimeDG> rallyetime(new RallyeTimeDG(config_file));

    cout<<"Entering main loop..."<<endl;
    rallyetime->run_till_quit();    
  }
  catch(exception& e)
  {
    cout<<"!!!"<<endl<<"Fatal exception: "<<e.what()<<endl<<"!!!"<<endl;    
  }

  cout<<endl<<"Goodbye!"<<endl;
  getline(cin,string());

  return 0;
}