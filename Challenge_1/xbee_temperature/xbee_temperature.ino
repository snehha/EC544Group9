#include <SoftwareSerial.h>
// which analog pin to connect
#define THERMISTORPIN A0         
#define NUMSAMPLES 5
#define SERIESRESISTOR 9100    
#define BCOEFFICIENT 3400
#define THERMISTORNOMINAL 4720      
#define TEMPERATURENOMINAL 25 
SoftwareSerial XBee(2, 3); // RX, TX
double floatVal = 1.2;
char charVal[10];
char access;
char key;

int samples[NUMSAMPLES];
bool join = false;

void initial_join() {
  XBee.write("{\"id\": 1, \"temp\": 75} \n");
  //XBee.write("Join1\n");
  //id "key_string"
  access = XBee.read();
  Serial.write(access);
  if(access == 'i') {
    while(XBee.available() > 0) {
      key = XBee.read();
      //Serial.write(key);
    }
    Serial.write(key);
    join = true;
  }
}

void setup(void) {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
}

void loop(void) {
  //Ask if join
  while(!join) {
    initial_join();
    delay(5000);
  }
  if(XBee.available() > 0){
    if((XBee.read()) == 'r'){
      join = false;
    }
  }
  
  uint8_t i;
  float average;
  
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;

  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;

  float f_temp = 0;
  float steinhart;
  steinhart = log (average / THERMISTORNOMINAL);     // ln (R/Ro)
  steinhart = steinhart/BCOEFFICIENT;               // ln(R/Ro)/B
  steinhart += (1.0 / (TEMPERATURENOMINAL + 273.15)); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart =  steinhart - 273.15;                         // convert to C
  f_temp = (steinhart * 9.0)/ 5.0 + 32.0;
  dtostrf(f_temp, 4, 2, charVal);
 // XBee.write("{ \"id\": %d, \"temp\": }")
  XBee.write(charVal);
  XBee.write("\n");
  delay(1000);
}
