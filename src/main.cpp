#include "RallyeTimeDG.h"

#include <iostream>
//#include <string> //included above in RallyeTimeDG
#include <memory>
using namespace std;


int main()
{
  const string config_file="data/settings.txt";

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