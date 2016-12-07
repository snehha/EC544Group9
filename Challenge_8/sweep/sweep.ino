#include <LIDARLite.h>

LIDARLite myLidarLite;

Servo lidarServo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

//#define NOTCONTINUOUS
#define CONTINUOUS

int sensorPins[] = {4,5}; // Array of pins connected to the sensor Power Enable lines
unsigned char addresses[] = {0x66,0x64};

int sensorPin = 4; // Array of pins connected to the sensor Power Enable lines

//Sensor Data
int sensorLeft;
int sensorRight;

void setup() {
  Serial.begin(9800);
  myLidarLite.begin();

  #ifdef CONTINUOUS
    myLidarLite.beginContinuous();
    pinMode(sensorPin, INPUT);
  #endif
  #ifdef NOTCONTINUOUS
    setAddress();
  #endif

  lidarServo.attach(A4);  // attaches the servo on pin 9 to the servo object
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
  for (int pos = 5; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    lidarServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(7);
    Serial.print(String(pos) + ",");                       // waits 15ms for the servo to reach the position
    Serial.println(String(sampleLidar(0)));
    delay(7);
  }
  for (int pos = 180; pos >= 5; pos -= 1) { // goes from 180 degrees to 0 degrees
    lidarServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(7);
    Serial.print(String(pos) + ",");                  // waits 15ms for the servo to reach the position
    Serial.println(String(sampleLidar(0)));
    delay(7);
  }


  delay(10);
  /*Serial.print("Lidar 2 distance: ");
  Serial.println(myLidarLite.distance(true,true,addresses[1]));*/
}

bool clockwise = true;

int sampleLidar(int sensorID) {
  int mySensor = 0;
  //for(int i = 0; i < 10; i++){
    #ifdef NOTCONTINUOUS
    mySensor = myLidarLite.distance(false,false,addresses[sensorID]);
    #endif
    #ifdef CONTINUOUS
    mySensor = myLidarLite.distanceContinuous();
    #endif
  //}
  return mySensor; ///10;
}

void getSensorData(int sensorID){

  for(int i = 0; i <= 180; i++){

  }
  lidarServo.write(180);
  delay(1260);
  sensorLeft = sampleLidar(sensorID);
  delay(10);
  Serial.println("Left reading: " + String(sensorLeft));

  lidarServo.write(0);
  delay(1260);
  sensorRight = sampleLidar(sensorID);
  delay(10);
  Serial.println("Right reading: " + String(sensorRight));
}

void loop() {
  moveServo();
  /*getSensorData(0);*/
}
