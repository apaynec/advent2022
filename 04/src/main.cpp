#include <Arduino.h>
#include "utils.h"

void setup() {
  Serial.begin(921600);
}

void loop() 
{
  static uint32_t t_last = millis();
  unsigned int tnow = millis();
  if (tnow - t_last > 5000)
  {
    t_last = tnow;
    uint32_t heap = ESP.getFreeHeap();
    Serial.printf("HO %lu\n",heap);
  }

  read_serial();
}