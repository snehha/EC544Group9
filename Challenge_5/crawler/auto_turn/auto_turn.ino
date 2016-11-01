

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


LIDARLite myLidarLite;

void setup() {
  Serial.begin(115200);

  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  
  myLidarLite.begin();
  myLidarLite.changeAddressMultiPwrEn(2,sensorPins,addresses,false);

  int i = 90;
  while(i != speed)
  {
    delay(45);
    esc.write(i--);
  }
  //calibrateESC();
}

/* Convert degree value to radians */
double degToRad(double degrees){
  return (degrees * 71) / 4068;
}

/* Convert radian value to degrees */
double radToDeg(double radians){
  return (radians * 4068) / 71;
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

void loop() {
  
  double newSpeed = speed;
  
  int sensor1 = 0;
  int sensor2 = 0;
  for(int i = 0; i < 10; i++){
    sensor1 += myLidarLite.distance(true,true,0x66);
    sensor2 += myLidarLite.distance(true,true,0x68);
  }
  sensor1 = sensor1/10;
  sensor2 = sensor2/10;

  Serial.println("About to print");
  if ((sensor1 < 50) || (sensor2 < 50)){
    //Serial.print(sensor1);
    //Serial.print(" ");
    //Serial.println(sensor2);
    newSpeed = 90;
    Serial.println("I WAS HERE");
  }
  else{
    newSpeed = 75;
  }

  double wheelOffsetLeft = 0;
  double wheelOffsetRight = 0;

  bool myCount = false;
  //Move Right
  if(sensor1 < 110){
    //Turn wheel X points
    //newSpeed = 75;
    myCount = true;
    wheelOffsetRight = -1 * maxWheelOffset * cos((pi*sensor1)/220);
  }
  //Move Left
  if(sensor2 < 110){
    //newSpeed = 75;
    myCount = true;
    wheelOffsetLeft = maxWheelOffset * cos((pi*sensor2)/220);
  }
  
  
//  //Control Speed
//  int stoppingDistance = 30;
//  if( (sensor1 < stoppingDistance) ) {
//      //newSpeed = (-1/2)*(sensor1)+87;
//      myCount = true;
//      newSpeed = 90;
//  }
//  //Control Speed
//  if( (sensor2 < stoppingDistance) ) {
//      myCount = true;
//      if (newSpeed < 90); //((-1/2)*(sensor2)+87))
//        newSpeed = 90;
//  }
  if(!myCount){
    newSpeed = 70;
  }

  
  
  double wheelOffset = wheelOffsetRight + wheelOffsetLeft;

  //esc.write(speed);
  wheels.write(90 + wheelOffset);
  
  

  //Serial.print("Speed: ");
  //Serial.println(speed);

}
