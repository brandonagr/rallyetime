#include "buttons_driver.h"
#include <time.h>
#include <windows.h>
#include <string>
#include <iostream>

using namespace std;

string convBase(unsigned long v, long base)
{
	string digits = "0123456789abcdef";
	string result;
	if((base < 2) || (base > 16)) {
		result = "Error: base out of range.";
	}
	else {
		do {
			result = digits[v % base] + result;
			v /= base;
		}
		while(v);
	}
	return result;
}

//================================================================
// 
ButtonsDriver::ButtonsDriver()
 :input_("task_button_input"),
  ws_input_("task_wheelsensor_input"),
  reset_("task_button_reset")
{

  //Initialize the LCD ports as outputs
  err_chk( DAQmxBaseCreateDIChan(input_.t_,"Dev1/port1/line4:7","",DAQmx_Val_ChanForAllLines) );
  err_chk( DAQmxBaseCreateCICountEdgesChan(ws_input_.t_,"Dev1/ctr0","",DAQmx_Val_Falling,0,DAQmx_Val_CountUp));
  err_chk( DAQmxBaseCreateDOChan(reset_.t_,"Dev1/port2/line6","",DAQmx_Val_ChanForAllLines) );
  err_chk( DAQmxBaseStartTask (input_.t_) );
  err_chk( DAQmxBaseStartTask (ws_input_.t_) );
  err_chk( DAQmxBaseStartTask (reset_.t_) );

  reset_buttons();    

  for(int i=0; i<100; i++)
  {
    /*
    unsigned char buttons=read_buttons();

    std::cout<<"Read in: "<<convBase(buttons,2)<<std::endl;

    if (buttons>3)
    {
      std::cout<<"Resetting debounce circuit..."<<std::endl;
      reset_buttons();
    }*/

    unsigned int count=read_wheelsensor();
    cout<<"wheel sensor count: "<<count<<endl;

    Sleep(1000);
  }
}
ButtonsDriver::~ButtonsDriver()
{
  //all handled automatically
}

//================================================================
// 
unsigned char ButtonsDriver::read_buttons()
{
  unsigned char data;
  int32 read;
  err_chk (DAQmxBaseReadDigitalU8 (input_.t_,1,1.0,DAQmx_Val_GroupByChannel,&data,1,&read,(bool32*)NULL));

  if (read==0)
    std::cout<<"What I didn't get any samples?"<<std::endl;

  return (data>>4)^3;
}

//================================================================
// 
unsigned long ButtonsDriver::read_wheelsensor()
{
  unsigned long data;
  err_chk (DAQmxBaseReadCounterScalarU32(ws_input_.t_,1.0,&data,NULL));

  return data;
}

//================================================================
// reset debounce circuit by going high then staying low 
void ButtonsDriver::reset_buttons()
{
  unsigned char data_reset=0;
  int32 written;
  err_chk (DAQmxBaseWriteDigitalU8(reset_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_reset,&written,(bool32*)NULL));
  data_reset=0xFF;
  err_chk (DAQmxBaseWriteDigitalU8(reset_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_reset,&written,(bool32*)NULL));
}