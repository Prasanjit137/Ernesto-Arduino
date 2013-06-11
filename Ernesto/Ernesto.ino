#include <LUltrasonic.h>
#include <LMotorController.h>
#include <LSharpIR.h>
#include <SoftwareSerial.h>


void measureDistances();
void findPathLoop();
void moveLoop();
void remoteControlLoop();


int forwardSpeed = 160;
int turningSpeed = 100;


//BLUETOOTH


char L_Data[4];
byte L_index = 0;
char R_Data[4];
byte R_index = 0;
char command;

#define cmdL 'L'
#define cmdR 'R'

char incomingByte;

#define RxD 12
#define TxD 13

SoftwareSerial blueToothSerial(RxD,TxD);

unsigned long lastTimeCommand, autoOFF = 1000;


//IR


unsigned int irFrontPin = A0;

LSharpIR irFront(irFrontPin);


//ULTRASONIC


unsigned long maxRange = 300;
unsigned long minDist = 30;
unsigned long moveMinDistSide = 40;
unsigned long moveMinDistFront = 50;

unsigned int echoPinLeft = 10;
unsigned int trigPinLeft = 11;

unsigned int echoPinFrontLeft = 9;
unsigned int trigPinFrontLeft = 8;

unsigned int echoPinFrontRight = A2;
unsigned int trigPinFrontRight = A3;

unsigned int echoPinRight = A4;
unsigned int trigPinRight = A5;


LUltrasonic ultrasonicLeft(echoPinLeft, trigPinLeft, maxRange);
LUltrasonic ultrasonicFrontLeft(echoPinFrontLeft, trigPinFrontLeft, maxRange);
LUltrasonic ultrasonicFrontRight(echoPinFrontRight, trigPinFrontRight, maxRange);
LUltrasonic ultrasonicRight(echoPinRight, trigPinRight, maxRange);


//MOTOR CONTROLLER


unsigned int ENA = 3;
unsigned int IN1 = 2;
unsigned int IN2 = 4;
unsigned int IN3 = 5;
unsigned int IN4 = 7;
unsigned int ENB = 6;

LMotorController motorController(ENA, IN1, IN2, ENB, IN3, IN4, 1, 1);


//STATES


typedef enum tagStateType
{
    StateTypeUnknown,
    StateTypeMove,
    StateTypeFindPath,
    StateTypeRemoteControl
}
StateType;

StateType currentStateType = StateTypeMove;
StateType previousStateType;


//SETUP


void setupBlueToothConnection()
{
    blueToothSerial.begin(9600);
    blueToothSerial.print("\r\n+STWMOD=0\r\n");
    blueToothSerial.print("\r\n+STNA=SeeedBTSlave\r\n");
    blueToothSerial.print("\r\n+STOAUT=1\r\n");
    blueToothSerial.print("\r\n+STAUTO=0\r\n");
    delay(2000);
    blueToothSerial.print("\r\n+INQ=1\r\n");
    delay(2000);
    blueToothSerial.flush();
}


void setup()
{
//    Serial.begin(9600);
    
    pinMode(RxD, INPUT);
    pinMode(TxD, OUTPUT);
    setupBlueToothConnection();
}


//LOOPS


void loop()
{
    if (currentStateType != StateTypeRemoteControl)
    {
        if (blueToothSerial.available())
            currentStateType = StateTypeRemoteControl;
        else
            measureDistances();
    }
    
    switch (currentStateType)
    {
        case StateTypeMove:
            moveLoop();
            break;
        case StateTypeFindPath:
            findPathLoop();
            break;
        case StateTypeRemoteControl:
            remoteControlLoop();
            break;
    }
}


void moveLoop()
{
    if (previousStateType != currentStateType)
    {
//        Serial.println("Move forward.");
        motorController.move(forwardSpeed);
        previousStateType = currentStateType;
    }
    
    if (ultrasonicLeft.getMeasuredDistance() <= minDist ||
        ultrasonicFrontLeft.getMeasuredDistance() <= minDist ||
        irFront.getMeasuredDistance() <= minDist ||
        ultrasonicFrontRight.getMeasuredDistance() <= minDist ||
        ultrasonicRight.getMeasuredDistance() <= minDist
        )
    {
        motorController.stopMoving();
        currentStateType = StateTypeFindPath;
    }
}


void findPathLoop()
{
    if (previousStateType != currentStateType)
    {
//        Serial.println("Find path.");
        (ultrasonicRight.getMeasuredDistance() <= minDist || ultrasonicFrontRight.getMeasuredDistance() <= minDist) ? motorController.turnLeft(turningSpeed, true) : motorController.turnRight(turningSpeed, true);
        previousStateType = currentStateType;
    }
    
    if (ultrasonicLeft.getMeasuredDistance() > moveMinDistSide &&
        ultrasonicFrontLeft.getMeasuredDistance() > moveMinDistSide &&
        irFront.getMeasuredDistance() > moveMinDistFront &&
        ultrasonicFrontRight.getMeasuredDistance() > moveMinDistSide &&
        ultrasonicRight.getMeasuredDistance() > moveMinDistSide
        )
    {
        motorController.stopMoving();
        currentStateType = StateTypeMove;
    }
}


void remoteControlLoop()
{
    if (blueToothSerial.available())
    {
        incomingByte = blueToothSerial.read();
        if(incomingByte == cmdL)
        {
            command = cmdL;
            memset(L_Data,0,sizeof(L_Data));
            L_index = 0;
        }
        else if(incomingByte == cmdR)
        {
            command = cmdR;
            memset(R_Data,0,sizeof(R_Data));
            R_index = 0;
        }
        else if(incomingByte == '\r')
            command = 'e';
        
        if (command == cmdL && incomingByte != cmdL)
        {
            L_Data[L_index] = incomingByte;
            L_index++;
        }
        else if(command == cmdR && incomingByte != cmdR)
        {
            R_Data[R_index] = incomingByte;
            R_index++;
        }
        else if(command == 'e')
        {
            int pwmLeft = atoi(L_Data);
            int pwmRight = atoi(R_Data);
            
            motorController.move(pwmLeft, pwmRight, 0);
            
            delay(10);
            
            lastTimeCommand = millis();
        }
    }
    
    if (millis() >= (lastTimeCommand + autoOFF))
    {
        motorController.stopMoving();
        currentStateType = StateTypeMove;
        previousStateType = StateTypeRemoteControl;
    }
}


//MEASURE DISTANCES


void measureDistances()
{
    ultrasonicLeft.measureDistance();
    delay(50);
    ultrasonicFrontRight.measureDistance();
    delay(50);
    irFront.measureDistance();
    delay(50);
    ultrasonicFrontLeft.measureDistance();
    delay(50);
    ultrasonicRight.measureDistance();
    delay(50);
    
//    Serial.print(ultrasonicLeft.getMeasuredDistance());
//    Serial.print("\t");
//    Serial.print(ultrasonicFrontLeft.getMeasuredDistance());
//    Serial.print("\t");
//    Serial.print(irFront.getMeasuredDistance());
//    Serial.print("\t");
//    Serial.print(ultrasonicFrontRight.getMeasuredDistance());
//    Serial.print("\t");
//    Serial.print(ultrasonicRight.getMeasuredDistance());
//    Serial.print("\t");
//    Serial.println("");
}