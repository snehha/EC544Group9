// which analog pin to connect
#define THERMISTORPIN A0         
#define NUMSAMPLES 5
// the value of the 'other' resistor
#define SERIESRESISTOR 9100    
 // The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3400
#define THERMISTORNOMINAL 4720      
#define TEMPERATURENOMINAL 25 

int samples[NUMSAMPLES];

void setup(void) {
  Serial.begin(9600);
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

  Serial.print("Average analog reading "); 
  Serial.println(average);

  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  Serial.print("Thermistor resistance "); 
  Serial.println(average);

  float f_temp = 0;
  float steinhart;
  steinhart = log (average / THERMISTORNOMINAL);     // ln (R/Ro)
  steinhart = steinhart/BCOEFFICIENT;               // ln(R/Ro)/B
  steinhart += (1.0 / (TEMPERATURENOMINAL + 273.15)); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart =  steinhart - 273.15;              // convert to C
  f_temp = (steinhart * 9.0)/ 5.0 + 32.0;       // Convert to F
  Serial.print("Temperature "); 
  Serial.print(f_temp);
  Serial.println(" Degrees F");
  
  delay(1000);
}
