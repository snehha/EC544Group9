// This #include statement was automatically added by the Particle IDE.
#include <LIDARLite.h>
#include "math.h"

String SSID;
String wifiData;

String BSSID = "";

Thread* wifiThread;
Thread* crawlerThread;

Servo lidarServo;

String ignoreWifiName = "Group9";

/******** Crawler variables ********/
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 35; // maximum wheel turn magnitude, in servo 'degrees'

int sensorPins[] = {4,5}; // Array of pins connected to the sensor Power Enable lines
unsigned char addresses[] = {0x66,0x64};
int frontLidar = 0;
int backLidar = 1;

const float pi = 3.14;

int disTurn = 10;
int disSpeed = 10;
int turnDir = 0;
int moveDir = 0;

//Speed values
int curSpeed = 0;
const int errDistance = 110;
const int stopped = 0;
const int fullSpeed = 20;
const int haltDistance = fullSpeed + 20;

//Spinning Servo values
bool clockwise = true;

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
int sensorNW;
int sensorNE;
int sensorWest;
int sensorEast;
int sensorNorth;
long frontUltrasound;

//Center distance on startup
int centerPoint;
int maxGapOffset = 100;
int medGapOffset = 30;
int centerBuffer;

LIDARLite myLidarLite;

void setupCrawler() {
    wheels.attach(D2); // initialize wheel servo to Digital IO Pin #8
    esc.attach(D3);    // initialize ESC to Digital IO Pin #9

    pinMode(4, INPUT);
    myLidarLite.begin();
    //myLidarLite.beginContinuous(true, 0x04, 0xff, addresses[frontLidar]);
    //setAddress();
    myLidarLite.beginContinuous();

    //Record the current center
    getSensorData(frontLidar);                  //Stored globally in sensorNW, sensorNE
    centerPoint = (sensorNW+sensorNE)/2;
    centerBuffer = centerPoint+medGapOffset;

    changeSpeed(fullSpeed);
}
/**************************************************/

//#define WIFISTUFF
//#define WEBSITECONTROL
//TODO Uncomment wifiThread
void setup() {
    Particle.variable("wifiData", &wifiData, STRING);

    // Website controls
    #ifdef WEBSITECONTROL
    Particle.function("moveCar", moveCar);
    Particle.function("startCar", startCar);
    #endif

    lidarServo.attach(A4);
    delay(50);
    setupCrawler();
    calibrateESC();

    #ifdef WIFISTUFF
    wifiThread = new Thread("sample", scanWifi);
    ignoreWifiName = WiFi.SSID();
    #endif

    crawlerThread = new Thread("sample", crawler);


}

void printLog(){
  Serial.println("--------- Current Values ---------");
  Serial.print("Left Sensor: ");
  Serial.println(sensorNW);
  Serial.print("Right Sensor: ");
  Serial.println(sensorNE);
  Serial.print("Center Distance: ");
  Serial.println(centerPoint);
  Serial.print("Front Sensor: ");
  Serial.println(frontUltrasound);
  Serial.print("Current Speed: ");
  Serial.println(curSpeed);
  Serial.print("Wheel Angle: ");
  Serial.println(curWheelAngle);
  Serial.print("Trim Angle: ");
  Serial.println(trimValue);
}


/***** website controls *****/
int moveCar(String direction) {
    // controls car
}

int startCar(String start) {
    // controls car
}

/************ Crawler Code ************/

void setAddress() {
  Serial.println("Establishing I2C connection...");
  String front, back;
  do {
    myLidarLite.changeAddressMultiPwrEn(2,sensorPins,addresses,false);
    front = myLidarLite.distance(true,true,addresses[0]);
    back = myLidarLite.distance(true,true,addresses[1]);
    Serial.println(front);
    Serial.println(back);
  } while ( front == "> nack"  ||  back == "> nack" );

  Serial.println("connection established.");
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


// TODO might get rid of
int sampleLidar(int sensorID) {
  int mySensor = 0;
  //for(int i = 0; i < 10; i++){
    //mySensor = myLidarLite.distance(false,false,addresses[sensorID]);
    mySensor = myLidarLite.distanceContinuous();
  //}
  return mySensor; ///10;
}

int degreeNW = 120;
int degreeW = 150;
int degreeNE = 60;
int degreeE = 30;

//TODO add halt threshold
void getSensorData(int sensorID){
  int pos, incr;
  sensorNW = 0;
  sensorNE = 0;
  sensorWest = 0;
  sensorEast = 0;

  if(clockwise){
      pos = 0;
      incr = 5;
  }
  else{
      pos = 180;
      incr = -5;
  }

  bool leftObjectDetected = false;
  bool rightObjectDetected = false;
  int sensorValue = 0;
  int ne = 1;
  int nw = 1;
  int w = 1;
  int e = 1;

  for (;(pos >= 0 && pos <=180); pos += incr) { // goes from 0 degrees to 90 degrees (Read left side)
      // in steps of 1 degree
      lidarServo.write(pos);               // tell servo to go to position in variable 'pos'
      delay(30);                        // waits 15ms for the servo to reach the position
      sensorValue = myLidarLite.distanceContinuous();

      if((pos >= degreeE) && (pos <= degreeNE)){  // North East
        ne++;
        if(!rightObjectDetected) {
          if(sensorValue < haltDistance) {
            sensorNE = sensorValue;
            rightObjectDetected = true;
            Serial.println("---Right object detected: ");
          } else {
            sensorNE += sensorValue;
          }
        }
      }
      else if(pos <= degreeE) { // East
        sensorEast += sensorValue;
        e++;
      }
      else if ((pos >= degreeNW) && (pos <= degreeW)){  // North West
        nw++;
        if(!leftObjectDetected) {
          if(sensorValue < haltDistance) {
            sensorNW = sensorValue;
            leftObjectDetected = true;
            Serial.println("---Left object detected: ");
          } else {
            sensorNW += sensorValue;
          }
        }
      }
      else if(pos >= degreeW) {
        w++;
        sensorWest += sensorValue;
      }

  }
  Serial.println("E: " + String(e) + " NE: " + String(ne) + " NW: " + String(nw) + " w: " + String(w));
  if(!leftObjectDetected) sensorNW = sensorNW / nw;
  if(!rightObjectDetected) sensorNE = sensorNE / ne;
  sensorEast = sensorEast / e;
  sensorWest = sensorWest / w;
  Serial.println("West reading: " + String(sensorWest));
  Serial.println("NW reading: " + String(sensorNW));
  Serial.println("NE reading: " + String(sensorNE));
  Serial.println("East reading: " + String(sensorEast));

  delay(10);

  clockwise = !clockwise;
}

void rLeft(){
  //Not turned left so make straight
  if(turnDir > 0){
    turnDir = 0;
  }
  //Turn more left
  else{
    turnDir -= disTurn;
  }
  changeWheelAngle(turnDir);
}

void rRight(){
  //Not turned right so make straight
  if(turnDir < 0){
    turnDir = 0;
  }
  //Turn more right
  else{
    turnDir += disTurn;
  }
  changeWheelAngle(turnDir);
}

void rForward(){
  //Moving backwards so stop
  if(moveDir < 0){
    moveDir = 0;
  }
  //Move faster
  else{
    moveDir += disSpeed;
  }
  changeSpeed(moveDir);
}

void rBackwards(){
  //Moving forwards so stop
  if(moveDir > 0){
    moveDir = 0;
  }
  //Move faster
  else{
    moveDir -= disSpeed;
  }
  changeReverseSpeed(moveDir);
}

void halt(){
  changeSpeed(0);
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

//TODO ADD TO THE BREADBOARD
void setTrim() {
  trimValue = analogRead(potPin);
  trimValue = map(trimValue, 0, 1018, -100, 100);
  trimValue /= 10;
}

//TODO IMPLEMENT
void getUltraSoundDistance() {
  //reset sample total
  sum = 0;
  for (int i = 0; i < avgRange ; i++) {
    analogVolt = analogRead(ultraPin) / 2;
    sum += analogVolt;
    delay(10);
  }

  inches = sum / avgRange;
  frontUltrasound = inches * 2.54;

}

//TODO Add reverse Sensor LIDAR readings here
void reverseCar(){

  /***
  * compare left and right. termerung pick the greater amount and
  * turn wheels opposite to that furthest direction.
  * back it up, back it up, back it up
  * watch out for back obstacles with backLidar....
  * set wheels back to oppsite direction ard and return to main
  ***/

  if( ((sensorNW/sensorNE) > 2) || ((sensorNW/sensorNE) < 0.5)){
    int wheelChange;
    Serial.println("Front object detected. Reversing.");
    if(sensorNW < sensorNE) //Go left
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
    int backTimers = 33*fullSpeed+600;
    while(backTimers != 0 || backLidar <= haltDistance + 20){
      backTimers--;
    }
    changeWheelAngle(-1*wheelChange);
  }

  //changeReverseSpeed(stopped);
}
//TODO reverseCar
bool stopCorrect() {
  if ( (frontUltrasound <= haltDistance+20) || ((sensorNW < haltDistance) && (sensorNE < haltDistance))) {
    Serial.println("Entered Stop Correct - SENSOR");
    changeSpeed(stopped);
    //reverseCar();
    Serial.println("Front object detected. Stopping.");
    return true;
  }
  else {
    changeSpeed(fullSpeed);
    return false;
  }
}

bool errorCorrect(int sensorNW, int sensorNE){
  double wheelOffsetLeft = 0;
  double wheelOffsetRight = 0;
  double wheelOffset = 0;
  bool myCount = false;

  //Move Right
  if(sensorNW < errDistance){
    Serial.println("Entered Error Correct - Move Right");
    myCount = true;
    wheelOffsetRight = -1 * maxWheelOffset * cos((pi*sensorNW)/2/errDistance);
  }
  //Move Left
  if(sensorNE < errDistance){
    Serial.println("Entered Error Correct - Move Left");
    //newSpeed = 75;
    myCount = true;
    wheelOffsetLeft = maxWheelOffset * cos((pi*sensorNE)/2/errDistance);
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

bool centerCorrect(int sensorNW, int sensorNE){
  double wheelOffsetLeft = 0;
  double wheelOffsetRight = 0;
  double wheelOffset = 0;

  int turnStrength = 5;

  //There is a big gap on the left side so stay to the right but in the center
  if(((sensorNW - sensorNE)>maxGapOffset) && (sensorNW > centerBuffer) /*&& (sensorNE > centerPoint)*/){
    Serial.println("Entered Center Correct - Gap Left");
    wheelOffset = -1 * maxWheelOffset * cos(pi/2*(sensorNE/centerPoint))/turnStrength;
  }
  //There is a gap on the right side so stay to the left
  else if(((sensorNE - sensorNW)>maxGapOffset) && (sensorNE > centerBuffer) /*&& (sensorNW > centerPoint)*/){
    Serial.println("Entered Center Correct - Gap Right");
    wheelOffset = maxWheelOffset * cos(pi/2*(sensorNW/centerPoint))/turnStrength;
  }

  //Center
  else{
    //Move Right
    if(sensorNW < centerPoint){
      Serial.println("Entered Center Correct - Center Right");
      wheelOffsetRight = -1 * curWheelAngle * cos(pi/2*(sensorNE/centerPoint))/turnStrength;
    }
    //Move Left
    if(sensorNE < centerPoint){
      Serial.println("Entered Center Correct - Center Left");
      wheelOffsetLeft = curWheelAngle * cos(pi/2*(sensorNW/centerPoint))/turnStrength;
    }
    wheelOffset = wheelOffsetRight + wheelOffsetLeft;
  }

  changeWheelAngle(wheelOffset);
}

//TODO looping crawler code, setTrim
void crawler() {
  while(true) {
    //TODO getUltraSoundDistance();
    getSensorData(frontLidar);
    delay(300);
    //setTrim();

    //printLog();

    /*bool val = stopCorrect();
    if(!val)
      val = errorCorrect(sensorNW,sensorNE);
    if(!val)
      centerCorrect(sensorNW,sensorNE);*/
  }
}
/****************************/


void moveServo(){
    //MAYBE REMOVE ME
}

void scanWifi() {
    while(true) {
        wifiData = "";
        String data = "";

        WiFiAccessPoint aps[20];
        int found = WiFi.scan(aps, 20);

        for (int i=0; i<found; i++) {
            BSSID = "";
            WiFiAccessPoint& ap = aps[i];

            if( ap.ssid != ignoreWifiName) {

                // BSSID string
                for(int j = 0; j < 6; j++) BSSID += String(ap.bssid[j]);

                SSID = ap.ssid;
                data += SSID + "," + BSSID + "," + String(ap.rssi) + "\n";
            }
        }
        wifiData = data;
        Serial.println(wifiData);
        delay(1500);
    }
}


/* Convert degree value to radians */
double degToRad(double degrees){
  return (degrees * 71) / 4068;
}

/* Convert radian value to degrees */
double radToDeg(double radians){
  return (radians * 4068) / 71;
}


void loop() {
}
