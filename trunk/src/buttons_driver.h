/* ================================================================

 ButtonsDriver class that manages buttons through a NI 6501 

Brandon Green - 08-05-29
================================================================= */
#ifndef BUTTONS_DRIVER_H_
#define BUTTONS_DRIVER_H_

#include "lcd_driver.h"


//----------------------------------------------------------------
class ButtonsDriver
{
private:
  //ports for lcd output
  DAQTask input_;
  DAQTask reset_;

  unsigned char read_buttons();
  void reset_buttons();  

public:
  ButtonsDriver();
  ~ButtonsDriver();



};


#endif

