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

char chr;
char temp[4];
//Will send 5 bytes every time -> either join or send temp/id
char data[4];
char go[4] = "hey";

int samples[NUMSAMPLES];
bool join;
int count;
uint16_t idRead = 0;

void setup(void) {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
  randomSeed(analogRead(1));
  pinMode(13,OUTPUT);
  digitalWrite(13,1);
  join = false;
}

void initial_join() {
  //Generate 2 random numbers
  uint16_t randNum = random(pow(2,16)-1);
  //Send 4 bytes
  char firstbyte = 255;
  char secondbyte = 255;
  char thirdbyte = randNum >> 8;
  char fourthbyte = randNum;
  sprintf(data,"%c%c%c%c",firstbyte,secondbyte,thirdbyte,fourthbyte);
  XBee.write(data);
  delay(5000);

  //Check if server is sending data
  bool serverRunning = false;
  if(XBee.available() > 0){
    serverRunning = true;
  }

  //Keep reading until you get the correct randNum
  int countRand = 0;
  int countId = 0;
  count = 0;
  
  while(serverRunning){
    digitalWrite(13,0);
    if(count++ == 1000)
      return;
    if(XBee.available() > 0){
      char byteRead = XBee.read();
      Serial.write(byteRead);

      //Receive msb first
      if(countRand == 0){
        if(byteRead == char(randNum >> 8))
          countRand++;
        continue;
      }
      if(countRand == 1){
        if(byteRead == char(randNum))
          countRand++;
        else
          countRand = 0;
        continue;
      }
  
      //Read ID
      if(countId == 0){
        idRead = (idRead | uint8_t(byteRead) << 8);
        countId++;
        continue;
      }
      if(countId == 1){
        idRead = (idRead | byteRead);
        Serial.print("Id is: ");
        Serial.print(idRead);
        Serial.print(" -> Successfully Joined\n");
        join = true;
        count = 0;
        break;
      }
    }
  }
}

void getTemp(){
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
  }
  digitalWrite(13,1);

  //Evaluates once it reads 4 bytes
  uint16_t val1, val2;
  //Read if data is available
  if(XBee.available() > 0){
    char byteRead = XBee.read();
    Serial.write("Byte ");
    Serial.print(count);
    Serial.print(" is: ");
    Serial.print(byteRead);
    Serial.print("\n");

    switch(count++){
      case 0:
        val1 = uint16_t(byteRead) << 8;
        break;
      case 1:
        val1 |= byteRead;
        break;
      case 2:
        val2 = uint16_t(byteRead) << 8;
        break;
      case 3:
        val2 |= byteRead;
        break;
    }
      
    if(count == 4)
      count = 0;

    else
      return;

    //00000000
    if(val1 == 0 && val2 == 0){
      join = false;
      Serial.write("Got Reset\n");
    }
    
    //00000001
    if(val1 == 0 && val2 == 1){
      Serial.write("Got Send\n");
      getTemp();
      
      //ID and Temp to send
      Serial.write("ID is ");
      Serial.print(idRead);
      Serial.write("\n");
      //char firstbyte = char(idRead >> 8);
      //char secondbyte = char(idRead);
      Serial.print(uint16_t(idRead));
      char firstbyte = 0;
      char secondbyte = 40;
      char thirdbyte = 74;
      char fourthbyte = 85;
      sprintf(data,"%c%c%c%c",firstbyte,secondbyte,thirdbyte,fourthbyte);
      XBee.write(data);
      //XBee.write("What");
    }
  }
}
