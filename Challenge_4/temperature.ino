PRODUCT_ID(1793);
PRODUCT_VERSION(1);
#include <math.h>

// which analog pin to connect
int THERMISTORPIN = A0;         
const int NUMSAMPLES = 5;
// the value of the 'other' resistor
int SERIESRESISTOR = 9100;    
 // The beta coefficient of the thermistor (usually 3000-4000)
int BCOEFFICIENT = 4000;
int THERMISTORNOMINAL = 9020;      
int TEMPERATURENOMINAL = 25;

String string_temperature;
float average = 0;
void setup(void) {
  Particle.variable("temperature", &string_temperature, STRING);
}
 
void loop(void) {
  uint8_t i;
  average = 0;
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   average = average + analogRead(A0);
   delay(10);
  }
  
  average = average / NUMSAMPLES;
  average = 4095 / average - 1;
  average = SERIESRESISTOR / average;
  
  float f_temp = 0;
  float steinhart;
  
  steinhart = log (average / THERMISTORNOMINAL);    // ln (R/Ro)
  steinhart = steinhart/BCOEFFICIENT;               // ln(R/Ro)/B
  steinhart = steinhart + (1.0 / (TEMPERATURENOMINAL + 273.15)); // + (1/To)
  steinhart = 1.0 / steinhart;                      // Invert
  steinhart =  steinhart - 273.15;                  // convert to C
  f_temp = (steinhart * 9.0)/ 5.0 + 32.0;
  string_temperature = String(f_temp, 2);
  Particle.publish("f_temperature", string_temperature);
  
  delay(2000);
}