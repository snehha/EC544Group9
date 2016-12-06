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


int sensorPins[] = {4,5}; // Array of pins connected to the sensor Power Enable lines
unsigned char addresses[] = {0x66,0x64};

int lidarPins[] = {4};

void setup() {
  Serial.begin(9800);
  myLidarLite.begin();
  setAddress();

  myservo.attach(A4);  // attaches the servo on pin 9 to the servo object
}

void setAddress() {
  Serial.println("Establishing I2C connection...");
  String test, test2;
  do {
    myLidarLite.changeAddressMultiPwrEn(2,sensorPins,addresses,false);
    test = myLidarLite.distance(true,true,addresses[0]);
    test2 = myLidarLite.distance(true,true,addresses[1]);
    Serial.println(test);
    Serial.println(test2);
  } while ( test == "> nack"  ||  test2 == "> nack" );

  Serial.println("connection established.");
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
  Serial.print("Lidar 0 distance: ");
  Serial.println(myLidarLite.distance(true,true,addresses[0]));
  delay(10);
  Serial.print("Lidar 2 distance: ");
  Serial.println(myLidarLite.distance(true,true,addresses[1]));
  //Serial.print(myLidarLite.read());
}

void loop() {
  moveServo();
}
