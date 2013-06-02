#ifndef LUltrasonic_h
#define LUltrasonic_h


#include <Arduino.h>


class LUltrasonic
{
  protected:
    unsigned int _echoPin, _triggerPin;
    unsigned long _maxRange;
    unsigned long _measuredDistance;
  public:
    LUltrasonic(unsigned int, unsigned int, unsigned long);
    unsigned long measureDistance();
    unsigned long measureDistance3();
    void setEchoPin(unsigned int);
    unsigned int getEchoPin();
    void setTriggerPin(unsigned int);
    unsigned int getTriggerPin();
    void setMaxRange(unsigned long);
    unsigned long getMaxRange();
    unsigned long getMeasuredDistance();
};


#endif