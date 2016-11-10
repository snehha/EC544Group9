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

/* Oscillate between various servo/ESC states, using a sine wave to gradually 
 *  change speed and turn values.
 */
void oscillate(){
  for (int i =0; i < 360; i++){
    double rad = degToRad(i);
    double speedOffset = sin(rad) * maxSpeedOffset;
    double wheelOffset = sin(rad) * maxWheelOffset;
    esc.write(90 + speedOffset);
    wheels.write(90 + wheelOffset);
    Serial.print("  ");
    Serial.println(wheelOffset);
    delay(50);
  }
  Serial.println(maxWheelOffset);
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

