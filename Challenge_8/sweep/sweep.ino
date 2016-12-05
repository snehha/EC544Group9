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

void setup() {
  Serial.begin(9800);
  myLidarLite.begin();
  myLidarLite.beginContinuous();
  pinMode(3, INPUT);

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
  Serial.println(myLidarLite.distanceContinuous());
  //Serial.println(myLidarLite.distance(true,true,0x66));
}

void loop() {
  moveServo();
}
