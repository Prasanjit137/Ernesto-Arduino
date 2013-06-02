#include "Arduino.h"
#include "LSharpIR.h"


LSharpIR::LSharpIR(unsigned int irPin)
{
    setIrPin(irPin);
}


unsigned int LSharpIR::measureDistance()
{
    float volts = analogRead(_irPin);
    float distance;
    
    if (volts > 500)
        distance = 10;
    else if (volts < 80)
        distance = 80;
    else
        distance = 4800/(volts - 20);
	
    _measuredDistance = (unsigned int)distance;
    
	return _measuredDistance;
}


unsigned int LSharpIR::measureDistance3()
{
	unsigned int a = LSharpIR::measureDistance();
	delay(50);
	unsigned int b = LSharpIR::measureDistance();
	delay(50);
	unsigned int c = LSharpIR::measureDistance();
	delay(50);
  
	unsigned int ab = pow(a-b,2);
	unsigned int bc = pow(b-c,2);
	unsigned int ac = pow(a-c,2);
  
	if (ab <= bc && ab <= ac)
		_measuredDistance = (a + b) / 2;
	else if (bc <= ab && bc <= ac)
		_measuredDistance = (b + c) / 2;
	else 
		_measuredDistance = (a + c) / 2;
		
	return _measuredDistance;
}


unsigned int LSharpIR::getMeasuredDistance()
{
    return _measuredDistance;
}


void LSharpIR::setIrPin(unsigned int irPin)
{
    _irPin = irPin;
	pinMode(_irPin, INPUT);
}


unsigned int LSharpIR::getIrPin()
{
    return _irPin;
}