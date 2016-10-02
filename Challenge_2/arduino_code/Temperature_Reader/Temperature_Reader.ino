#include <SoftwareSerial.h>
// which analog pin to connect
#define THERMISTORPIN A0         
#define NUMSAMPLES 5
// the value of the 'other' resistor
#define SERIESRESISTOR 9100    
 // The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3400
#define THERMISTORNOMINAL 4720      
#define TEMPERATURENOMINAL 25 
SoftwareSerial XBee(2, 3); // RX, TX

int samples[NUMSAMPLES];
char temp[10] = "";
char dataSend[4]= "";
char randBuff[4] = "";
char float_t[10] = "";
 
void setup(void) {
  Serial.begin(9600);
  XBee.begin(9600);
}
 
void loop(void) {
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
  int find_period;
  
  steinhart = log (average / THERMISTORNOMINAL);     // ln (R/Ro)
  steinhart = steinhart/BCOEFFICIENT;               // ln(R/Ro)/B
  steinhart += (1.0 / (TEMPERATURENOMINAL + 273.15)); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart =  steinhart - 273.15;                         // convert to C
  f_temp = (steinhart * 9.0)/ 5.0 + 32.0;
  String string_temperature = String(f_temp, 2);
//  dtostrf(f_temp, 4, 2, temp);
//  Serial.println(f_temp);
//  String string_temperature = String(temp);
  find_period = string_temperature.indexOf('.');
  String whole_temp = string_temperature.substring(0,find_period);
  String float_temp = string_temperature.substring(find_period+1);

  char whole_t = int(whole_temp);
  char float_t = int(float_temp);
  
  char firstbyte = B10000000;
  char secondbyte = char(id);
  if(id >= 255){
    firstbyte = char(id >> 8);
  }
  //Send 4 bytes of data [id_high, id_low, whole number of temp, decimal portion of temp]
  sprintf(dataSend,"%c%c%c%c", firstbyte, secondbyte, whole_t, float_t);
  //Transmit Data via XBEE to Node
  XBee.write(dataSend);
    
  delay(2000);
}
