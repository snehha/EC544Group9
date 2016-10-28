

#include <Wire.h>
#include <LIDARLite.h>
#include <Servo.h>
 
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 60; // maximum wheel turn magnitude, in servo 'degrees'

int sensorPins[] = {2,3}; // Array of pins connected to the sensor Power Enable lines
unsigned char addresses[] = {0x66,0x68};
const float pi = 3.14;

LIDARLite myLidarLite;

void setup() {
  Serial.begin(115200);

  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  
  myLidarLite.begin();
  myLidarLite.changeAddressMultiPwrEn(2,sensorPins,addresses,false);

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

void loop() {

  int sensor1 = 0;
  int sensor2 = 0;
  for(int i = 0; i < 40; i++){
    sensor1 += myLidarLite.distance(true,true,0x66);
    sensor2 += myLidarLite.distance(true,true,0x68);
  }
  sensor1 = sensor1/40;
  sensor2 = sensor2/40;

  Serial.print(sensor1);
  Serial.print(" ");
  Serial.println(sensor2);

  double wheelOffsetLeft = 0;
  double wheelOffsetRight = 0;
  //Move Right
  if(sensor1 < 90){
    //Turn wheel X points
    wheelOffsetRight = -1 * maxWheelOffset * cos((pi*sensor1)/180);
  }

  //Move Left
  if(sensor2 < 90){
    wheelOffsetLeft = maxWheelOffset * cos((pi*sensor2)/180); 
  }

  double wheelOffset = wheelOffsetRight + wheelOffsetLeft;

  esc.write(78);
  wheels.write(90 + wheelOffset);

  Serial.print("Wheel Degree: ");
  Serial.println(wheelOffset);

}
