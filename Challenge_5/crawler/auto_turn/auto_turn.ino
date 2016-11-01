#include <Wire.h>
#include <LIDARLite.h>
#include <Servo.h>
 
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 60; // maximum wheel turn magnitude, in servo 'degrees'

int sensorPins[] = {6,7}; // Array of pins connected to the sensor Power Enable lines
unsigned char addresses[] = {0x66,0x68};
const float pi = 3.14;

//Speed values
int curSpeed = 15;
const int errDistance = 110;
const int haltDistance = 50;
const int stopped = 0;
const int fullSpeed = 15;

//Wheel values
float trimValue;
double curWheelAngle;

//Sensor Data
int sensorLeft;
int sensorRight;
float sensorFront;

//Center distance on startup
int centerPoint;
int maxGapOffset = 50;
int medGapOffset = 30;
int centerBuffer;

LIDARLite myLidarLite;

void setup() {
  Serial.begin(115200);

  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  
  myLidarLite.begin();
  myLidarLite.changeAddressMultiPwrEn(2,sensorPins,addresses,false);

  //Set speed to max speed
  changeSpeed(fullSpeed);
  //calibrateESC();

  //Record the current center
  getSensorData(sensorLeft,sensorRight);
  centerPoint = (sensorLeft+sensorRight)/2;
  centerBuffer = centerPoint+medGapOffset;
}

/* Calibrate the ESC by sending a high signal, then a low, then middle.*/
void calibrateESC(){
    esc.write(180); // full backwards
    delay(startupDelay);
    esc.write(0); // full forwards
    delay(startupDelay);
    esc.write(90); // neutral
    delay(startupDelay);
    esc.write(90); // reset the ESC to neutral (non-moving) value
}

int getSensorData(int &sensor1, int &sensor2){
  int mySensor1 = 0;
  int mySensor2 = 0;
  for(int i = 0; i < 10; i++){
    mySensor1 += myLidarLite.distance(true,true,0x66);
    mySensor2 += myLidarLite.distance(true,true,0x68);
  }
  sensor1 = mySensor1/10;
  sensor2 = mySensor2/10;
}

//Change the speed of the rover up or down.
void changeSpeed(int newSpeed){

  //curSpeed = The actual speed of the rover at the moment
  //fullSpeed = The full speed of the rover
  // 90 - newSpeed >> get the real speed

  //if the newSpeed is greater than the curSpeed then we are speeding up
  // 20 > 0
  if(newSpeed > curSpeed){
    while(curSpeed != newSpeed)
    {
      curSpeed++;
      esc.write(90-curSpeed);
      delay(45);
    }
  }
  //If the newSpeed is less than the curSpeed then we are slowing down
  // 0 < 20
  else if(newSpeed < curSpeed){
    while(curSpeed != newSpeed)
    {
      curSpeed--;
      esc.write(90-curSpeed);
      delay(45);
    }
  }
}

//Change the wheel angle
void changeWheelAngle(double newWheelAngle){
  curWheelAngle = 90 + trimValue + newWheelAngle;
  wheels.write(curWheelAngle);
  
}

void printLog(){
  Serial.println("--------- Current Values ---------");
  Serial.print("Left Sensor: ");
  Serial.println(sensorLeft);
  Serial.print("Right Sensor: ");
  Serial.println(sensorRight);
  Serial.print("Front Sensor: ");
  Serial.println(sensorFront);
  Serial.print("Current Speed: ");
  Serial.println(curSpeed);
  Serial.print("Wheel Angle: ");
  Serial.println(curWheelAngle);
  Serial.print("Trim Angle: ");
  Serial.println(trimValue);
}

bool stopCorrect(int sensor1, int sensor2){
  //STOP CORRECT- Done by the ultra sonic sensor
  if ((sensor1 < haltDistance) && (sensor2 < haltDistance))
    changeSpeed(stopped);
}

bool errorCorrect(int sensorLeft, int sensorRight){
  double wheelOffsetLeft = 0;
  double wheelOffsetRight = 0;
  bool myCount = false;
  
  //Move Right
  if(sensorLeft < errDistance){
    myCount = true;
    wheelOffsetRight = -1 * maxWheelOffset * cos((pi*sensorLeft)/2/errDistance);
  }
  //Move Left
  if(sensorRight < errDistance){
    //newSpeed = 75;
    myCount = true;
    wheelOffsetLeft = maxWheelOffset * cos((pi*sensorRight)/2/errDistance);
  }
  //If not close to wall, go set max speed
  if(!myCount){
    changeSpeed(fullSpeed);
  }
  
  double wheelOffset = wheelOffsetRight + wheelOffsetLeft;
  changeWheelAngle(wheelOffset);
}

bool centerCorrect(int sensorLeft, int sensorRight){
  double wheelOffsetLeft = 0;
  double wheelOffsetRight = 0;
  double wheelOffset = 0;
  
  //There is a big gap on the left side so stay to the right but in the center
  if(((sensorLeft - sensorRight)>maxGapOffset) && (sensorLeft > centerBuffer) /*&& (sensorRight > centerPoint)*/){
    wheelOffset = -1 * maxWheelOffset * cos(pi/2*(sensorRight/centerPoint));
  }
  //There is a gap on the right side so stay to the left
  else if(((sensorRight - sensorLeft)>maxGapOffset) && (sensorRight > centerBuffer) /*&& (sensorLeft > centerPoint)*/){
    wheelOffset = maxWheelOffset * cos(pi/2*(sensorLeft/centerPoint));
  }
  
  //Center
  else{
    //Move Right
    if(sensorLeft < centerPoint){
      wheelOffsetRight = -1 * maxWheelOffset * cos(pi/2*(sensorRight/centerPoint));
    }
    //Move Left
    if(sensorRight < centerPoint){
      wheelOffsetLeft = maxWheelOffset * cos(pi/2*(sensorLeft/centerPoint));
    }
    wheelOffset = wheelOffsetRight + wheelOffsetLeft;
  }
  
  changeWheelAngle(wheelOffset);
}

void loop() {  
  getSensorData(sensorLeft,sensorRight);
  
  bool val = stopCorrect(sensorLeft,sensorRight);
  if(!val)
    val = errorCorrect(sensorLeft,sensorRight);
  if(!val)
    centerCorrect(sensorLeft,sensorRight);
}
