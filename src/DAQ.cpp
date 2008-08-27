#include "DAQ.h"


//----------------------------------------------------------------
// 
LCDDriver::LCDDriver()
 :p0_("task_lcd_p0"),
  p1_("task_lcd_p1")
{

  //Initialize the LCD ports as outputs
  err_chk( DAQmxCreateDOChan(p0_.t_,"Dev1/port0","",DAQmx_Val_ChanForAllLines) );
  err_chk( DAQmxCreateDOChan(p1_.t_,"Dev1/port1/line0:2","",DAQmx_Val_ChanForAllLines) );
  err_chk( DAQmxStartTask (p0_.t_) );
  err_chk( DAQmxStartTask (p1_.t_) );

  //need to initialize variables on display
  write_data(0x01); //clear display
  write_data(0x02); //return to home position
  write_data(0x06); //set entry mode
  write_data(0x0c); //set display cursor on for now, 0x0f=blink with underscore, 0x0d=blink no underscore, 0x0e=just the underline
  write_data(0x38); //set data size and enable full screen

  //write_string("Brandon + Merissa :)");  

  
  for(int i=0; i<800; i++)
  {
    write_string("-------------------|-------------------|-------------------|------------------X");    
  }
  
}
LCDDriver::~LCDDriver()
{
  //all handled automatically
}


//----------------------------------------------------------------
// 
void LCDDriver::write_data(unsigned char data, bool rs)
{
  unsigned char data_p0=data;
  unsigned char data_p1=(rs?0x1:0x0) | 0x4; //set enable pin high
  int32 written;

  err_chk (DAQmxWriteDigitalU8(p0_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_p0,&written,(bool32*)NULL));
  err_chk (DAQmxWriteDigitalU8(p1_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_p1,&written,(bool32*)NULL));

  data_p1=(rs?0x1:0x0); //set enable pin low to latch data above

  err_chk (DAQmxWriteDigitalU8(p1_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_p1,&written,(bool32*)NULL));
}
void LCDDriver::write_string(std::string line)
{
  for(std::string::iterator i=line.begin(); i!=line.end(); i++)
  {
    write_data((unsigned char)*i,true);    
  }
}



//----------------------------------------------------------------
// 
void err_chk(int ret)
{
  if (DAQmxFailed(ret))
  {
    char error[1024];
    DAQmxGetExtendedErrorInfo(error,1024);      
    throw DAQException(std::string(error));
  }
}





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
  err_chk( DAQmxCreateDIChan(input_.t_,"Dev1/port1/line4:7","",DAQmx_Val_ChanForAllLines) );
  err_chk( DAQmxCreateCICountEdgesChan(ws_input_.t_,"Dev1/ctr0","",DAQmx_Val_Falling,0,DAQmx_Val_CountUp));
  err_chk( DAQmxCreateDOChan(reset_.t_,"Dev1/port2/line6","",DAQmx_Val_ChanForAllLines) );
  err_chk( DAQmxStartTask (input_.t_) );
  err_chk( DAQmxStartTask (ws_input_.t_) );
  err_chk( DAQmxStartTask (reset_.t_) );

  reset_buttons();    

  for(int i=0; i<100; i++)
  {
    
    unsigned char buttons=read_buttons();

    std::cout<<"Read in: "<<convBase(buttons,2)<<std::endl;

    if (buttons>3)
    {
      std::cout<<"Resetting debounce circuit..."<<std::endl;
      reset_buttons();
    }
/*
    unsigned int count=read_wheelsensor();
    cout<<"wheel sensor count: "<<count<<endl;
*/
    Sleep(1000);
  }
}
ButtonsDriver::~ButtonsDriver()
{
  //all handled automatically
}

//----------------------------------------------------------------
// 
unsigned char ButtonsDriver::read_buttons()
{
  unsigned char data;
  int32 read;
  err_chk (DAQmxReadDigitalU8 (input_.t_,1,1.0,DAQmx_Val_GroupByChannel,&data,1,&read,(bool32*)NULL));

  if (read==0)
    std::cout<<"What I didn't get any samples?"<<std::endl;

  return (data>>4)^3;
}

//----------------------------------------------------------------
// 
unsigned long ButtonsDriver::read_wheelsensor()
{
  unsigned long data;
  err_chk (DAQmxReadCounterScalarU32(ws_input_.t_,1.0,&data,NULL));

  return data;
}

//----------------------------------------------------------------
// reset debounce circuit by going high then staying low 
void ButtonsDriver::reset_buttons()
{
  unsigned char data_reset=0;
  int32 written;
  err_chk (DAQmxWriteDigitalU8(reset_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_reset,&written,(bool32*)NULL));
  data_reset=0xFF;
  err_chk (DAQmxWriteDigitalU8(reset_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_reset,&written,(bool32*)NULL));
}