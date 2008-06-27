#include "lcd_driver.h"
#include <time.h>
#include <windows.h>

//================================================================
// 
LCDDriver::LCDDriver()
 :p0_("task_lcd_p0"),
  p1_("task_lcd_p1")
{

  //Initialize the LCD ports as outputs
  err_chk( DAQmxBaseCreateDOChan(p0_.t_,"Dev1/port0","",DAQmx_Val_ChanForAllLines) );
  err_chk( DAQmxBaseCreateDOChan(p1_.t_,"Dev1/port1/line0:2","",DAQmx_Val_ChanForAllLines) );
  err_chk( DAQmxBaseStartTask (p0_.t_) );
  err_chk( DAQmxBaseStartTask (p1_.t_) );

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


//================================================================
// 
void LCDDriver::write_data(unsigned char data, bool rs)
{
  unsigned char data_p0=data;
  unsigned char data_p1=(rs?0x1:0x0) | 0x4; //set enable pin high
  int32 written;

  err_chk (DAQmxBaseWriteDigitalU8(p0_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_p0,&written,(bool32*)NULL));
  err_chk (DAQmxBaseWriteDigitalU8(p1_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_p1,&written,(bool32*)NULL));

  data_p1=(rs?0x1:0x0); //set enable pin low to latch data above

  err_chk (DAQmxBaseWriteDigitalU8(p1_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_p1,&written,(bool32*)NULL));
}
void LCDDriver::write_string(std::string line)
{
  for(std::string::iterator i=line.begin(); i!=line.end(); i++)
  {
    write_data((unsigned char)*i,true);    
  }
}



//================================================================
// 
void err_chk(int ret)
{
  if (DAQmxFailed(ret))
  {
    char error[1024];
    DAQmxBaseGetExtendedErrorInfo(error,1024);      
    throw DAQException(std::string(error));
  }
}