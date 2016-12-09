// This #include statement was automatically added by the Particle IDE.
#include <LIDARLite.h>
#include "math.h"

String SSID;
String wifiData;

String BSSID = "";

Thread* wifiThread;
Thread* crawlerThread;
Thread* serialThread;

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

bool autopilot = true;

//Speed values
int curSpeed = 0;
const int errDistance = 110;
const int stopped = 0;
const int fullSpeed = 20;
const int haltDistance = fullSpeed + 20;
const int ultraHalt = haltDistance + 240;

//Spinning Servo values
bool clockwise = true;

//Wheel values
float trimValue = 0;
double curWheelAngle = 0;

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

//Knn variables
String serialCommand; //Left or Right
String turnCommand;
String leftKey = "knnLeft";
String rightKey = "knnRight";
bool cornerSoon = false;
bool initTurnLid = true;
int inTurnRadius = 30;
int prevLidar;
int lidarCalc;
const double turnRatio = 2.0;

int stopCorrectMax = 0;

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
    getSensorData();                  //Stored globally in sensorNW, sensorNE
    centerPoint = (sensorNW+sensorNE)/2;
    centerBuffer = centerPoint+medGapOffset;

    changeSpeed(fullSpeed);
    changeWheelAngle(0);
}
/**************************************************/

#define WIFISTUFF
//TODO Uncomment wifiThread
void setup() {
    Particle.variable("wifiData", &wifiData, STRING);

    lidarServo.attach(A7);
    delay(50);
    calibrateESC();
    setupCrawler();

    #ifdef WIFISTUFF
    wifiThread = new Thread("sample", scanWifi);
    ignoreWifiName = WiFi.SSID();
    #endif

    crawlerThread = new Thread("sample", crawler);
    serialThread = new Thread("sample",serialComm);
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
  Serial.print("KNN Reading: ");
  Serial.println(trimValue);

}

/***** website control *****/
int moveCar(String direction) {
    // receives command from photon
    if(direction == "up") {
      autopilot = false;
      rForward();
      Serial.println("Pi Command received: " + direction);
    }
    else if (direction == "down") {
      autopilot = false;
      rBackward();
      Serial.println("Pi Command received: " + direction);
    }
    else if (direction == "left") {
      autopilot = false;
      rLeft();
      Serial.println("Pi Command received: " + direction);
    }
    else if (direction == "right") {
      autopilot = false;
      rRight();
      Serial.println("Pi Command received: " + direction);
    }
    else if (direction == "start") {
      autopilot = false;
      //changeSpeed(fullSpeed);
      Serial.println("Pi Command received: " + direction);
    }
    else if (direction == "stop") {
      autopilot = false;
      changeSpeed(stopped);
      turnDir = 0;
      moveDir = 0;
      Serial.println("Pi Command received: " + direction);
    }
    else if (direction == "autopilotOn") {
      Serial.println("Pi Command received: "+ direction);
      if(!autopilot) {
        Serial.println("Starting autopilot");
        autopilot = true;
        cornerSoon = false;
        stopCorrectMax = 0;
        crawlerThread = new Thread("sample", crawler);
      }
    }
    else if (direction == "autopilotOff") {
      Serial.println("Pi Command received: " + direction);
      autopilot = false;
    }


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

int degreeNW = 120;
int degreeW = 150;
int degreeNE = 60;
int degreeE = 30;

// TODO add halt threshold
void getSensorData(){
  int pos, incr;
  sensorNW = 0;
  sensorNE = 0;
  sensorWest = 0;
  sensorEast = 0;
  sensorNorth = 0;

  if(clockwise){
      pos = 20;
      incr = 5;
  }
  else{
      pos = 170;
      incr = -5;
  }

  bool leftObjectDetected = false;
  bool rightObjectDetected = false;
  bool northObjectDetected = false;
  int sensorValue = 0;
  int ne = 1;
  int nw = 1;
  int w = 1;
  int e = 1;
  int n = 1;

  for (; (pos >= 20 && pos <=170); pos += incr) { // goes from 0 degrees to 90 degrees (Read left side)
      if(cornerSoon) return;
      //setLidarPos(pos);                         // tell servo to go to position in variable 'pos'
      lidarServo.write(pos);                    // tell servo to go to position in variable 'pos'
      delay(20);                                // waits 15ms for the servo to reach the position
      sensorValue = myLidarLite.distanceContinuous();

      if((pos >= degreeE) && (pos <= degreeNE)){  // North East
        ne++;
        if(!rightObjectDetected) {
          if(sensorValue < haltDistance) {        // if object is detected, do not take average
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
          if(sensorValue < haltDistance) {    // if object is detected, do not take average
            sensorNW = sensorValue;
            leftObjectDetected = true;
            Serial.println("---Left object detected: ");
          } else {
            sensorNW += sensorValue;
          }
        }
      }
      else if(pos >= degreeW) { // West
        w++;
        sensorWest += sensorValue;
      }
      else if((pos >= degreeNE) && (pos <= degreeNW)){  // North
        n++;
        if(!northObjectDetected) {
          if(sensorValue < haltDistance) {  // if object is detected, do not take average
            sensorNorth = sensorValue;
            northObjectDetected = true;
            Serial.println("---North object detected: ");
          } else {
            sensorNorth += sensorValue;
          }
        }
      }

  }
  //Serial.println("E: " + String(e) + " NE: " + String(ne) + " N: " + String(n) + " NW: " + String(nw) + " w: " + String(w));
  if(!leftObjectDetected) sensorNW = sensorNW / nw;
  if(!rightObjectDetected) sensorNE = sensorNE / ne;
  if(!northObjectDetected) sensorNorth = sensorNorth / n;
  sensorEast = sensorEast / e;
  sensorWest = sensorWest / w;
  Serial.println("West reading: " + String(sensorWest));
  Serial.println("NW reading: " + String(sensorNW));
  Serial.println("North reading: " + String(sensorNorth));
  Serial.println("NE reading: " + String(sensorNE));
  Serial.println("East reading: " + String(sensorEast));

  clockwise = !clockwise;
}

//Change the speed of the rover up or down.
void changeSpeed(int newSpeed){
  // Serial.println("Entering changeSpeed");
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

// TODO ADD TO THE BREADBOARD
void setTrim() {
  trimValue = analogRead(potPin);
  trimValue = map(trimValue, 0, 1018, -100, 100);
  trimValue /= 10;
}

// TODO IMPLEMENT
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
  Serial.println("UltraSound Dist: "+String(frontUltrasound));

}

// TODO Add reverse Sensor LIDAR readings here
void reverseCar(){

  /***
  * compare left and right. termerung pick the greater amount and
  * turn wheels opposite to that furthest direction.
  * back it up, back it up, back it up
  * watch out for back obstacles with backLidar....
  * set wheels back to oppsite direction ard and return to main
  ***/

  stopCorrectMax++;

  getSensorData();

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
    while(backTimers != 0 || frontUltrasound < ultraHalt + 20){
      backTimers--;
    }
    changeWheelAngle(-1*wheelChange);
  }

  //changeReverseSpeed(stopped);
}
// TODO reverseCar
bool stopCorrect() {
  getUltraSoundDistance();
  if ( (frontUltrasound <= ultraHalt + 20) || ((sensorNW < haltDistance) && (sensorNE < haltDistance))) {
    //Serial.println("Entered Stop Correct - SENSOR");
    changeSpeed(stopped);
    if(stopCorrectMax <= 3)
    {
      reverseCar();
    }
    Serial.println("Front object detected. Stopping.");
    return true;
  }
  else {
    changeSpeed(fullSpeed);
    stopCorrectMax = 0;
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

bool hugWall(){
  if(turnCommand == leftKey) {
    changeWheelAngle(-5);
    return true;
  }
  else if(turnCommand == rightKey) {
    changeWheelAngle(5);
    return true;
  }

  return false;
}

void setLidarPos(int newPos){
  int currentPosition = lidarServo.read();
  lidarServo.write(newPos);
  delay(abs(currentPosition-newPos)*5);
}

void setLidarDir(){
    if(turnCommand == leftKey){
      setLidarPos(170);
      lidarCalc = myLidarLite.distanceContinuous();
      sensorNE = errDistance + (errDistance - lidarCalc);
      sensorNW = lidarCalc;
      initTurnLid = false;
    }
    else if(turnCommand == rightKey){
      setLidarPos(20);
      sensorNW = errDistance + (errDistance - lidarCalc);
      sensorNE = lidarCalc;
      initTurnLid = false;
    }

    if (initTurnLid) {
      Serial.println("Forcing " + turnCommand);
      prevLidar = myLidarLite.distanceContinuous();
    }

}

void turnCorner(){
  //int lidarCalc = myLidarLite.distanceContinuous();
  //Serial.println("Corner lidar Dist: " + String(lidarCalc));
  if(turnCommand == rightKey && (lidarCalc/prevLidar > turnRatio)){
    changeWheelAngle(inTurnRadius);
    //turnCommand = "";
  }
  else if(turnCommand == leftKey && (lidarCalc/prevLidar > turnRatio)){
    changeWheelAngle(inTurnRadius*-1);
    //turnCommand = "";
  }
}

// TODO looping crawler code, setTrim
void crawler() {
  while(autopilot) {
    //if(autopilot) {
      Serial.println("Autopilot");
      getUltraSoundDistance();
      if(!cornerSoon){
        getSensorData();
        Serial.println("got sensor data");
      }
      else{
        setLidarDir();
      }
      //setTrim();

      //printLog();

      bool val = stopCorrect();
      while(val) {
        val = stopCorrect();
      }
      if(!val) Serial.println("stopCorrect false");
      /*if(!val){
        val = hugWall();
      }*/
      if(!val){
        val = errorCorrect(sensorNW,sensorNE);
      }
      /******MIGHT OVERWRITE OVER ERROR CORRECT********/
      if(cornerSoon && !val){
        turnCorner();
        continue;
      }
      if(!val){
        centerCorrect(sensorNW,sensorNE);
      }
      Serial.println("------------------------");
    }
  //}
}

/****************************/
//Communicate using socket on python side
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
        Particle.publish("wifiDataHook", wifiData);
        //Serial.println(wifiData);
        delay(1500);
    }
}

/***************Manual Car Control****************/
void rRight(){
  //Not turned left so make straight
  if(turnDir > 0){
    turnDir = 0;
  }
  //Turn more left
  else if(abs(turnDir) <= maxWheelOffset){
    turnDir -= disTurn;
  }
  changeWheelAngle(turnDir);
}

void rLeft(){
  //Not turned right so make straight
  if(turnDir < 0){
    turnDir = 0;
  }
  //Turn more right
  else if(abs(turnDir) <= maxWheelOffset){
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

void rBackward(){
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
/*********************************************/
void serialComm(){
  while(true){
    serialCommand = "";
    while(Serial.available() > 0 ){
      char byteRead = Serial.read();
      if(byteRead != '\n')
        serialCommand.concat(char(byteRead));
    }
    if(serialCommand != "")
      Serial.println("Command Received:" + serialCommand);
    if(serialCommand == leftKey || serialCommand == rightKey){
      turnCommand = serialCommand;
      Serial.println("Have to turn soon");
      cornerSoon = true;
      initTurnLid = true;
    } else if(serialCommand == "stopTurn") { // Compass direction "Comp:___"
      cornerSoon = false;
    }
    else{
      moveCar(serialCommand);
    }
  }
}

void loop() {
}
