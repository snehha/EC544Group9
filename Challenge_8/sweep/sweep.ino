/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <LIDARLite.h>

LIDARLite myLidarLite;

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position


int sensorPins[] = {6}; // Array of pins connected to the sensor Power Enable lines
unsigned char addresses[] = {0x62};

void setup() {
  Serial.begin(9800);
  myLidarLite.begin();
  //myLidarLite.beginContinuous();
  //myLidarLite.beginContinuous( true,0x04,0xff, addresses[0]);
  myLidarLite.changeAddressMultiPwrEn(1,sensorPins,addresses,false);
  //pinMode(3, INPUT);

  myservo.attach(A4);  // attaches the servo on pin 9 to the servo object
}

void moveServo() {
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(7);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(7);                       // waits 15ms for the servo to reach the position
  }
  Serial.print("Lidar distance: ");
  //Serial.println(myLidarLite.distanceContinuous());
  //Serial.println(myLidarLite.distanceContinuous(addresses[0]));
  Serial.println(myLidarLite.distance(true,true,addresses[0]));
}

void loop() {
  moveServo();
}
