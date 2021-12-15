/* ~~~~~~~~~~ Includes ~~~~~~~~~~ */
#include "display.h"

//#define SER_DEBUG
unsigned long timer_ref;
const unsigned long timer_offset = 40;

void setup() {

  display_setup();
  timer_ref = millis();

}

void loop() {

  if(millis() >= timer_ref + timer_offset){
    timer_ref = millis();
    display_loop();
  }
  
}
