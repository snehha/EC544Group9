#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX
char strBuff;
int id = 0;
bool join = false;
float temp = 10.0;


void setup() {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
}

void loop() {
  while(!join){
    XBee.write("Permission to Join");
  }
  else{
    XBee.write("ID:%d,%f",id,temp);
  }
    while(XBee.available() > 0){
      strBuff = XBee.read();
      Serial.write(strBuff);
    }
    XBee.write("An Xbee 4 Says Hi\n");
    delay(2000);
  }
  else{
    
  }
}
