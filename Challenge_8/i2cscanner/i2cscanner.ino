// --------------------------------------
// i2c_scanner
//
// Version 1
//    This program (or code that looks like it)
//    can be found in many places.
//    For example on the Arduino.cc forum.
//    The original author is not know.
// Version 2, Juni 2012, Using Arduino 1.0.1
//     Adapted to be as simple as possible by Arduino.cc user Krodal
// Version 3, Feb 26  2013
//    V3 by louarnold
// Version 4, March 3, 2013, Using Arduino 1.0.3
//    by Arduino.cc user Krodal.
//    Changes by louarnold removed.
//    Scanning addresses changed from 0...127 to 1...119,
//    according to the i2c scanner by Nick Gammon
//    http://www.gammon.com.au/forum/?id=10896
// Version 5, March 28, 2013
//    As version 4, but address scans now to 127.
//    A sensor seems to use address 120.
// Version 6, November 27, 2015.
//    Added waiting for the Leonardo serial communication.
//
//
// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.
//

#include "application.h"

//#include <LIDARLite.h>

//LIDARLite myLidarLite;

int lidarPins[] = {4};
unsigned char addresses[] = {0x66,0x64};

/*void setup() {
  Serial.begin(9800);
  myLidarLite.begin();
  myLidarLite.changeAddressMultiPwrEn(2,sensorPins,addresses,false);

  myservo.attach(A4);  // attaches the servo on pin 9 to the servo object
}*/


void setup()
{

  int sensorPins[] = {4,5}; // Array of pins connected to the sensor Power Enable lines


  Wire.begin();
  /*myLidarLite.begin();
  myLidarLite.changeAddress(0x62, false, 0x64);
  myLidarLite.beginContinuous();
  pinMode(sensorPins[0], INPUT);
*/
  //myLidarLite.changeAddressMultiPwrEn(2,sensorPins,addresses,false);

  Serial.begin(9600);
  Serial.println("test");
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nI2C Scanner");
}


void loop()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      //Serial.print("Lidar 0 distance: ");
      //Serial.println(myLidarLite.distance(true,true,addresses[0]));
    //  Serial.println(myLidarLite.distanceContinuous());
/*      delay(10);
      Serial.print("Lidar 2 distance: ");
      Serial.println(myLidarLite.distance(true,true,addresses[1]));
*/
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknow error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}