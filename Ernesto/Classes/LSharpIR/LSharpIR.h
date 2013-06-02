#ifndef LSharpIR_h
#define LSharpIR_h


#include <Arduino.h>


class LSharpIR
{
  protected:
    unsigned int _irPin;
    unsigned int _measuredDistance;
 public:
    LSharpIR(unsigned int);
    unsigned int measureDistance();
    unsigned int measureDistance3();
    unsigned int getMeasuredDistance();
    void setIrPin(unsigned int irPin);
    unsigned int getIrPin();
};


#endif