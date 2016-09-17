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
int random_number;


int samples[NUMSAMPLES];
bool join = true;

void initial_join() {
  //XBee.write("{\"id\": 1, \"temp\": 75}\n");
  random_number = random(1,10000);
  XBee.write("J");                  // Send a join message to XBee
  XBee.write(random_number);
  XBee.write("\n");
  access = XBee.read();             // Receive an "i" to get the id number
  Serial.write(access);             // Debugging
  if(access == 'i') {
    while(XBee.available() > 0) {   // if access is "i", 
      key = XBee.read();            // then get the is number
      //Serial.write(key);
    }
    Serial.write(key);              // Debugging
    join = true;                    // Continue with the temperature readout
  }
}

void get_temp(){
  //Temperature code begins
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
  // Calculating the temperature
  float steinhart;
  steinhart = log (average / THERMISTORNOMINAL);     // ln (R/Ro)
  steinhart = steinhart/BCOEFFICIENT;               // ln(R/Ro)/B
  steinhart += (1.0 / (TEMPERATURENOMINAL + 273.15)); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart =  steinhart - 273.15;             // convert to C
  f_temp = (steinhart * 9.0)/ 5.0 + 32.0;      //Convert to F
  // Converting from float to a character
  dtostrf(f_temp, 4, 2, charVal);
  Serial.write(charVal);
  Serial.write("\n");
//  XBee.write("{ \"id\": ");
//  XBee.write(key);
//  XBee.write("\"temp\": ");
//  XBee.write(charVal);
//  XBee.write("}");
//  XBee.write("\n");
  delay(1000);
}
void setup(void) {
  XBee.begin(9600);
  Serial.begin(9600);
}

void loop(void) {
  // Ask to join initially
  while(!join) {
    initial_join();
    delay(5000);
  }
  // If the coordinator wants to reset
  if(XBee.available() > 0){
    if((XBee.read()) == 'r'){
      join = false;
    }
  }
  get_temp();
  delay(1000);
}
