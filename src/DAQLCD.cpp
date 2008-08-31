#include "DAQLCD.h"


//----------------------------------------------------------------
// 
DAQLCDThread::DAQLCDThread(bool* kill_flag)
 :kill_flag_(kill_flag),
  p0_("task_lcd_p0"),
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
  write_data(0x14); //set entry mode
  write_data(0x0c); //set display cursor on for now, 0x0f=blink with underscore, 0x0d=blink no underscore, 0x0e=just the underline, 0x0c=no cursor
  write_data(0x38); //set data size and enable full screen

  internal_write_string(LCDString(0,0,"+------------------+"));
  internal_write_string(LCDString(0,1,"|   Initializing   |"));
  internal_write_string(LCDString(0,2,"|    RallyeTime    |"));
  internal_write_string(LCDString(0,3,"+------------------+"));
}
DAQLCDThread::~DAQLCDThread()
{
  write_data(0x01); //clear display  
}


//----------------------------------------------------------------
// 
void DAQLCDThread::write_data(unsigned char data, bool rs)
{
  unsigned char data_p0=data;
  unsigned char data_p1=(rs?0x1:0x0) | 0x4; //set enable pin high
  int32 written;

  err_chk (DAQmxWriteDigitalU8(p0_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_p0,&written,(bool32*)NULL));
  err_chk (DAQmxWriteDigitalU8(p1_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_p1,&written,(bool32*)NULL));

  data_p1=(rs?0x1:0x0); //set enable pin low to latch data above

  err_chk (DAQmxWriteDigitalU8(p1_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_p1,&written,(bool32*)NULL));
}

//----------------------------------------------------------------
// 
void DAQLCDThread::set_location(unsigned char x, unsigned char y)
{
  if (y==1)
    y=2;
  else if (y==2)
    y=1;
  else if (y==3) //bizarre mapping
  {
    y=4;
    x+=4;
  }

  //std::cout<<"moving to location: ("<<(int)x<<", "<<(int)y<<")"<<std::endl;
  //std::cout<<"address offset: "<<(int)((y*20+x)&0x7F)<<std::endl;

  // take number and mask with 01111111, then OR with 10000000 to get in correct set ddram address mode
  unsigned char data_p0=0x80 | ((y*20+x)&0x7F);
  unsigned char data_p1=0x4; //enable pin high
  int32 written;

  err_chk (DAQmxWriteDigitalU8(p0_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_p0,&written,(bool32*)NULL));
  err_chk (DAQmxWriteDigitalU8(p1_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_p1,&written,(bool32*)NULL));

  data_p1=0;; //set enable pin low to latch data above

  err_chk (DAQmxWriteDigitalU8(p1_.t_,1,0,1.0,DAQmx_Val_GroupByChannel,&data_p1,&written,(bool32*)NULL));

}

//----------------------------------------------------------------
// 
void DAQLCDThread::internal_write_string(LCDString& data)
{
  set_location(data.x_,data.y_);

  for(std::string::iterator i=data.data_.begin(); i!=data.data_.end(); i++)
  {
    write_data((unsigned char)*i,true);
    //std::cout<<"writing "<<*i<<" "<<(int)*i<<std::endl;
  }
}


//----------------------------------------------------------------
// 
void DAQLCDThread::run()
{
  bool need_write;
  LCDString tempdata;

  while(!(*kill_flag_))
  {
    //check if anything in write que    

    do
    {
      need_write=false;

      {
        scoped_lock lock(shared_data_mutex_);
        
        if (write_que_.size()>0)
        {
          need_write=true;
          tempdata=write_que_.front();
          write_que_.pop_front();
        }
      }

      if (need_write)
        internal_write_string(tempdata);

    }
    while(need_write);

    boost::thread::yield();
  }
}




//----------------------------------------------------------------
// 
void DAQLCDThread::write_string(LCDString& data)
{
  scoped_lock lock(shared_data_mutex_);

  if (write_que_.size()>12) //hmmm, silently throwing away lcd updates is probably not a good thing!  
    write_que_.pop_front();  

  write_que_.push_back(data);
}