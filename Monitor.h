#ifndef MONITOR_H
#define MONITOR_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define MonitorLightPin 2

class {
public:
  void init() {
    pinMode(MonitorLightPin, OUTPUT);
  }

  void lightUp() {
    digitalWrite(MonitorLightPin, HIGH);
  }

  void lightDown() {
    digitalWrite(MonitorLightPin, LOW);
  }
  
} Monitor;

#endif  // MONITOR_H
