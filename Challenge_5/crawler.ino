#include <Servo.h>
 
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'

const double MAXDISTANCE = 1000;
double simulateLeftRAND = 500;
double simulateRightRAND = 500;
const int ultraPin = 0;

long analogVolt, inches, cm;
int sum = 0; //Create sum variable so it can be averaged
int avgRange = 60; //Quantity of values to average (sample size)


void setup()
{
  Serial.begin(9600);
 
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
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

long getUltraSoundDistance(){
  //reset sample total
  sum = 0;
  for (int i = 0; i < avgrange ; i++) {
    //Used to read in the analog voltage output that is being sent by the MaxSonar device.
    //Scale factor is (Vcc/512) per inch. A 5V supply yields ~9.8mV/in
    //Arduino analog pin goes from 0 to 1024, so the value has to be divided by 2 to get the actual inches
    analogVolt = analogRead(ultraPin) / 2;
    sum += analogVolt;
    delay(10);
  }

  inches = sum / avgRange;
  cm = inches * 2.54;

  Serial.print("Ultrasound values: ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  
}

void simulate(){
  
  double diffAmount = random(10,70);

  
  double picker = random(0,10);
  if(picker > 5){
    if( (simulateRightRAND - diffAmount) < 0 ){
      simulateRightRAND = 0;
      simulateLeftRAND = MAXDISTANCE;
    }
    else{
      simulateLeftRAND += diffAmount ;
      simulateRightRAND -= diffAmount;
    }
  }
  else{
    if( (simulateLeftRAND - diffAmount) < 0 ){
      simulateRightRAND = MAXDISTANCE;
      simulateLeftRAND = 0;
    }
    else{
      simulateLeftRAND -= diffAmount ;
      simulateRightRAND += diffAmount;
    }
  }

  Serial.print("Left: " + String(simulateLeftRAND));
  Serial.println(" Right: " + String(simulateRightRAND));
}


 
void loop()
{
   //oscillate();
   simulate();
}

