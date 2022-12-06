#include <Arduino.h>
#include <M5Core2.h>

#include "utils.h"

void setup() 
{
  M5.begin(false,false,false,false);

  Serial.begin(921600);
  //Serial.begin(500000);
  Serial.flush();
  delay(50);
}

//void serialEvent()
//{
//  read_serial();
//}

void loop() 
{
  /*
  static uint32_t t_last = millis();
  unsigned int tnow = millis();
  if (tnow - t_last > 5000)
  {
    t_last = tnow;
    uint32_t heap = ESP.getFreeHeap();
    Serial.printf("HO %lu\n",heap);
  }
  */

  read_serial();

  if (state_bof)
  {
    state_bof = false;
    start_of_file();
  }
  if (state_eof)
  {
    state_eof = false;
    end_of_file();
  }
  if (state_eol)
  {
    state_eol = false;
    // process_line();
  }
}