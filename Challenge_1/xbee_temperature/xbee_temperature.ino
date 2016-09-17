#include <SoftwareSerial.h>
#include <Math.h>
// which analog pin to connect
#define THERMISTORPIN A0         
#define NUMSAMPLES 5
#define SERIESRESISTOR 9100    
#define BCOEFFICIENT 3400
#define THERMISTORNOMINAL 4720      
#define TEMPERATURENOMINAL 25
SoftwareSerial XBee(2, 3); // RX, TX
//Only 4 precision
char temp[4];
//Reading one char at a time
char chr;
//Support for 1,000 devices at most
char id[3] = "";
char randBuff[4] = "";
char dataSend[30];
int count;

int randNum;

int samples[NUMSAMPLES];
bool join = false;

void clearArr(char *arr,int n){
  for(int i = 0; i < n; i++)
    arr[i] = '\0';
}

void setup(void) {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
  randomSeed(analogRead(0));
}

void initial_join() {
  Serial.write("Init\n");
  clearArr(id,3);
  
  //Generate Random Number
  randNum = random(1000);

  //Ask to Join
  String data = "j" + String(randNum) + "\n";
  data.toCharArray(dataSend,6);
  XBee.write(dataSend);

  //Keep reading until you get an id
  bool readId = false;
  bool readComma = false;
  while(true){
    if(XBee.available() > 0){
      chr = XBee.read();
      //Serial.write(chr);
      
      if(!readId){
        if(chr == 'i'){
          readId = true;
        }
        continue;
      }
  
      //Get returned RandNum
      if(!readComma){
        if(chr == ','){
          randBuff[count] = '\0';
          readComma = true;
          count = 0;
          continue;
        }
        randBuff[count] = chr;
        count++;
        continue;
      }
      
      //Check if id is meant for me
      if(atoi(randBuff) == randNum){
        //Get your id
        if(chr == '\n'){
          id[count] = '\0';
          join = true;
          Serial.write(id);
          Serial.write('\n');
          Serial.write("Joined");
          Serial.write('\n');
          break;
        }
        id[count] = chr;
        count++;
      }
      else{
        readId = false;
        readComma = false;
      }
    }
  }
}

void getTemp(char *Arr){
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
  dtostrf(f_temp, 4, 2, temp);
}

void loop(void) {
  //Ask to join and get unique id
  while(!join) {
    initial_join();
    join = true;
  }
  
  //Read if data is available
  if(XBee.available() > 0){
    chr = XBee.read();
    Serial.write("Data Received: ");
    Serial.write(chr);
    Serial.write('\n');

    //Coordinator sends out r when it's restarted so no one is registered
    if(chr == 'r'){
      join = false;
      Serial.write("WTFFFFF");
    }
    
    if(chr == 's'){
      getTemp(temp);
      Serial.write("Temp: ");
      Serial.write(temp);
      Serial.write('\n');

      //Send one long string terminated by null
      String data = "{ \"id\": " + String(id) + ", \"temp\": " + String(temp) + " }\n";
      data.toCharArray(dataSend,30);
      XBee.write(dataSend);
      /*XBee.write("{ \"id\": ");
      XBee.write(id);
      XBee.write(", \"temp\": ");
      XBee.write(temp);
      XBee.write(" }\n");*/
      /*String data = String(id) + "\n";
      data.toCharArray(dataSend,2);
      XBee.write(dataSend);*/
      //Might need to delay to prevent buffer overflow
      //delay(randNum(1000));
    }
  }
}
