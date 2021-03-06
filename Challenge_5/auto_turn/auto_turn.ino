#include <Wire.h>
#include <LIDARLite.h>
#include <Servo.h>
#include <SoftwareSerial.h>
 
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
int curSpeed = 0;
const int errDistance = 110;
const int stopped = 0;
const int fullSpeed = 20;
const int haltDistance = fullSpeed + 20;

//Wheel values
float trimValue = 0;
double curWheelAngle;

//Ultrasonic
const int ultraPin = 0;
const int potPin = 1;
long analogVolt, inches;
int sum = 0; //Create sum variable so it can be averaged
int avgRange = 10; //Quantity of values to average (sample size)

//Sensor Data
int sensorLeft;
int sensorRight;
long sensorFront;

//Center distance on startup
int centerPoint;
int maxGapOffset = 100;
int medGapOffset = 30;
int centerBuffer;

//XBEE Output
SoftwareSerial XBee(2, 3);

LIDARLite myLidarLite;

void setup() {
  Serial.begin(115200);
  XBee.begin(9600);

  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  
  myLidarLite.begin();
  myLidarLite.changeAddressMultiPwrEn(2,sensorPins,addresses,false);

  //calibrateESC();
  //Set speed to max speed

  //Record the current center
  getSensorData(sensorLeft,sensorRight);
  centerPoint = (sensorLeft+sensorRight)/2;
  centerBuffer = centerPoint+medGapOffset;

  changeSpeed(fullSpeed);
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
  Serial.println("Entering changeSpeed");
  //curSpeed = The actual speed of the rover at the moment
  //fullSpeed = The full speed of the rover
  // 90 - newSpeed >> get the real speed
  if(newSpeed == stopped){
    curSpeed = stopped;
    esc.write(90);
    return;
  }
  //if the newSpeed is greater than the curSpeed then we are speeding up
  // 20 > 0
  if(newSpeed > curSpeed){
    while(curSpeed != newSpeed)
    {
      curSpeed++;
      esc.write(90-curSpeed);
      delay(25);
    }
  }
  //If the newSpeed is less than the curSpeed then we are slowing down
  // 0 < 20
  else if(newSpeed < curSpeed){
    while(curSpeed != newSpeed)
    {
      curSpeed--;
      esc.write(90-curSpeed);
      delay(25);
    }
  }
}

void changeReverseSpeed(int newSpeed){
  //curSpeed = The actual speed of the rover at the moment
  //fullSpeed = The full speed of the rover
  // 90 - newSpeed >> get the real speed
  if(newSpeed == stopped){
    curSpeed = stopped;
    esc.write(90);
    return;
  }
  //if the newSpeed is greater than the curSpeed then we are speeding up
  // 20 > 0
  if(newSpeed < curSpeed){
    Serial.print("Entering changeSpeed: ");
    Serial.println(newSpeed);
    while(curSpeed != newSpeed)
    {
      curSpeed--;
      esc.write(90-curSpeed);
      delay(25);
      Serial.print("Speed new: ");
      Serial.println(curSpeed);
    }
  }
  //If the newSpeed is less than the curSpeed then we are slowing down
  // 0 < 20
  else if(newSpeed > curSpeed){
    Serial.println("Entering changeSpeed Bottom");
    while(curSpeed != newSpeed)
    {
      curSpeed++;
      esc.write(90-curSpeed);
      delay(25);
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
  Serial.print("Center Distance: ");
  Serial.println(centerPoint);
  Serial.print("Front Sensor: ");
  Serial.println(sensorFront);
  Serial.print("Current Speed: ");
  Serial.println(curSpeed);
  Serial.print("Wheel Angle: ");
  Serial.println(curWheelAngle);
  Serial.print("Trim Angle: ");
  Serial.println(trimValue);
}

void XBeePrint(){
  XBee.write("--------- Current Values ---------\n");
  XBee.write("\nLeft Sensor: ");
  XBee.write(sensorLeft);
  XBee.write("\nRight Sensor: ");
  XBee.write(sensorRight);
  XBee.write("\nCenter Distance: ");
  XBee.write(centerPoint);
  XBee.write("\nFront Sensor: ");
  XBee.write(sensorFront);
  XBee.write("\nCurrent Speed: ");
  XBee.write(curSpeed);
  XBee.write("\nWheel Angle: ");
  XBee.write(curWheelAngle);
  XBee.write("\nTrim Angle: ");
  XBee.write(trimValue);
}

void setTrim() {
  trimValue = analogRead(potPin);
  trimValue = map(trimValue, 0, 1018, -100, 100);
  trimValue /= 10;
}

void getUltraSoundDistance() {
  //reset sample total
  sum = 0;
  for (int i = 0; i < avgRange ; i++) {

    analogVolt = analogRead(ultraPin) / 2;
    sum += analogVolt;
    delay(10);
  }

  inches = sum / avgRange;
  sensorFront = inches * 2.54;

}

void reverseCar(){
  if( ((sensorLeft/sensorRight) > 2) || ((sensorLeft/sensorRight) < 0.5)){
    int wheelChange;
    Serial.println("Front object detected. Reversing.");
    if(sensorLeft < sensorRight) //Go left
    {
      wheelChange = maxWheelOffset - 5;
      changeWheelAngle(wheelChange);
      changeReverseSpeed(-15);
    }
    else{
      wheelChange = -maxWheelOffset + 5;
      changeWheelAngle(wheelChange);
      changeReverseSpeed(-15);
    }
    delay(33*fullSpeed+500);
    changeWheelAngle(-1*wheelChange);
  }
  
  //changeReverseSpeed(stopped);
}

bool stopCorrect(long sensorFront) {
  if ( (sensorFront <= haltDistance+20) || ((sensorLeft < haltDistance) && (sensorRight < haltDistance))) {
    Serial.println("Entered Stop Correct - SENSOR");
    changeSpeed(stopped);
    reverseCar();
    Serial.println("Front object detected. Stopping.");
    return true;
  }
  else {
    changeSpeed(fullSpeed);
    return false;
  }
}

bool errorCorrect(int sensorLeft, int sensorRight){
  double wheelOffsetLeft = 0;
  double wheelOffsetRight = 0;
  double wheelOffset = 0;
  bool myCount = false;
  
  //Move Right
  if(sensorLeft < errDistance){
    Serial.println("Entered Error Correct - Move Right");
    myCount = true;
    wheelOffsetRight = -1 * maxWheelOffset * cos((pi*sensorLeft)/2/errDistance);
  }
  //Move Left
  if(sensorRight < errDistance){
    Serial.println("Entered Error Correct - Move Left");
    //newSpeed = 75;
    myCount = true;
    wheelOffsetLeft = maxWheelOffset * cos((pi*sensorRight)/2/errDistance);
  }
  //If not close to wall, go set max speed
  if(!myCount){
    Serial.println("Entered Error Correct - Stay Course");
    changeSpeed(fullSpeed);
    //changeWheelAngle(wheelOffset);
    return false;
  }
  
  wheelOffset = wheelOffsetRight + wheelOffsetLeft;
  changeWheelAngle(wheelOffset);
  return true;
}

bool centerCorrect(int sensorLeft, int sensorRight){
  double wheelOffsetLeft = 0;
  double wheelOffsetRight = 0;
  double wheelOffset = 0;

  int turnStrength = 5;
  
  //There is a big gap on the left side so stay to the right but in the center
  if(((sensorLeft - sensorRight)>maxGapOffset) && (sensorLeft > centerBuffer) /*&& (sensorRight > centerPoint)*/){
    Serial.println("Entered Center Correct - Gap Left");
    wheelOffset = -1 * maxWheelOffset * cos(pi/2*(sensorRight/centerPoint))/turnStrength;
  }
  //There is a gap on the right side so stay to the left
  else if(((sensorRight - sensorLeft)>maxGapOffset) && (sensorRight > centerBuffer) /*&& (sensorLeft > centerPoint)*/){
    Serial.println("Entered Center Correct - Gap Right");
    wheelOffset = maxWheelOffset * cos(pi/2*(sensorLeft/centerPoint))/turnStrength;
  }
  
  //Center
  else{
    //Move Right
    if(sensorLeft < centerPoint){
      Serial.println("Entered Center Correct - Center Right");
      wheelOffsetRight = -1 * curWheelAngle * cos(pi/2*(sensorRight/centerPoint))/turnStrength;
    }
    //Move Left
    if(sensorRight < centerPoint){
      Serial.println("Entered Center Correct - Center Left");
      wheelOffsetLeft = curWheelAngle * cos(pi/2*(sensorLeft/centerPoint))/turnStrength;
    }
    wheelOffset = wheelOffsetRight + wheelOffsetLeft;
  }
  
  changeWheelAngle(wheelOffset);
}

void loop() {  
  getSensorData(sensorLeft,sensorRight);
  getUltraSoundDistance();
  setTrim();
  
  printLog();
  //XBeePrint();
  bool val = stopCorrect(sensorFront);
  if(!val)
    val = errorCorrect(sensorLeft,sensorRight);
  if(!val)
    centerCorrect(sensorLeft,sensorRight);
}
