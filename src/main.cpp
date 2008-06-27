#include <iostream>

#include "lcd_driver.h"
#include "buttons_driver.h"

using namespace std;



void main()
{
  try
  {
    //LCDDriver lcd;
    ButtonsDriver buttons;
  }
  catch(DAQException& e)
  {
    cout<<"Failed with message: "<<e.m_<<endl;
  }
}