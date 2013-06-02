#include "Arduino.h"
#include "LUltrasonic.h"


LUltrasonic::LUltrasonic(unsigned int echoPin, unsigned int triggerPin, unsigned long maximumRange)
{
    setTriggerPin(triggerPin);
	setEchoPin(echoPin);
    setMaxRange(maximumRange);
}


unsigned long LUltrasonic::measureDistance()
{
	unsigned long duration, distance; 
	
	digitalWrite(_triggerPin, LOW); 
	delayMicroseconds(10); 
	digitalWrite(_triggerPin, HIGH); 
	delayMicroseconds(20); 
	digitalWrite(_triggerPin, LOW); 
	duration = pulseIn(_echoPin, HIGH); 
	distance = duration / 29 / 2;
	
	_measuredDistance = distance <= _maxRange ? distance : _maxRange;
	
	return _measuredDistance;
}


unsigned long LUltrasonic::measureDistance3()
{
	unsigned long a = LUltrasonic::measureDistance();
	delay(50);
	unsigned long b = LUltrasonic::measureDistance();
	delay(50);
	unsigned long c = LUltrasonic::measureDistance();
	delay(50);
  
	unsigned long ab = pow(a-b,2);
	unsigned long bc = pow(b-c,2);
	unsigned long ac = pow(a-c,2);
  
	if (ab <= bc && ab <= ac)
		_measuredDistance = (a + b) / 2;
	else if (bc <= ab && bc <= ac)
		_measuredDistance = (b + c) / 2;
	else 
		_measuredDistance = (a + c) / 2;
		
	return _measuredDistance;
}


void LUltrasonic::setEchoPin(unsigned int echoPin)
{
 	_echoPin = echoPin;
	pinMode(_echoPin, INPUT);
}


unsigned int LUltrasonic::getEchoPin()
{
    return _echoPin;
}


void LUltrasonic::setTriggerPin(unsigned int triggerPin)
{
	_triggerPin = triggerPin;
	pinMode(_triggerPin, OUTPUT);
}


unsigned int LUltrasonic::getTriggerPin()
{
    return _triggerPin;
}


void LUltrasonic::setMaxRange(unsigned long maxRange)
{
    _maxRange = maxRange;
}


unsigned long LUltrasonic::getMaxRange()
{
    return _maxRange;
}


unsigned long LUltrasonic::getMeasuredDistance()
{
    return _measuredDistance;
}