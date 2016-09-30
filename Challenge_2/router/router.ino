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

//Reading one char at a time
char chr;
//Max temp 255.255
char temp[2] = "";
//Support for 2^16 devices at most
char id[2] = "";
char randBuff[4] = "";
//Will send 5 bytes every time -> either join or send temp/id
char data[5];

char firstbyte;
char secondbyte;
char thirdbyte;
char fourthbyte;

int counter;

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
  randomSeed(analogRead(1));
  pinMode(13,OUTPUT);
  join = false;
}

void initial_join() {
  counter = 0;
  
  //Generate Random Number max 2^16-1
  char randNum1 = random(pow(2,7)-1);
  char randNum2 = random(pow(2,7)-1);
  //Ask to Join [FFFF][randNum]
  clearArr(data,5);
  firstbyte = B1111111;
  secondbyte = B1111111;
  thirdbyte = randNum1;
  fourthbyte = randNum2;
  sprintf(data,"%c%c%c%c\n",firstbyte,secondbyte,thirdbyte,fourthbyte);
  XBee.write(data);
  delay(5000);

  //Check if server is sending data
  bool serverRunning = false;
  if(XBee.available() > 0)
    serverRunning = true;

  //Keep reading until you get the correct randNum
  int countRand = 0;
  uint16_t randRead = 0;
  int countId = 0;
  uint16_t idRead = 0;
  
  while(serverRunning){
    digitalWrite(13,0);
    if(counter++ == 1000)
      return;
    if(XBee.available() > 0){
      uint16_t byteRead = XBee.read();
      char sender[10];
      sprintf(sender,"--%u--\n",byteRead);
      Serial.write(sender);

      //Receive msb first
      if(countRand == 0){
        randRead = (randRead | byteRead << 8 );
        countRand++;
        continue;
      }
      if(countRand == 1){
        randRead = (randRead | byteRead);
        countRand++;
      }
      //If randNum read is not mine then it was meant for other arduino
      if(randRead != randNum1){
        countRand = 0;
        continue;
      }
  
      //Read ID
      if(countId == 0){
        idRead = (idRead | byteRead << 8);
        countId++;
        continue;
      }
      if(countId == 1){
        idRead = (idRead | byteRead);
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
    if(join){
      break;
    }
    //Wait before asking again to join
    delay(2000);
  }
  //Read if data is available
  if(XBee.available() > 0){
    chr = XBee.read();
    Serial.write(chr);

    //Coordinator sends out r when it's restarted so no one is registered
    if(chr == 'r'){
      join = false;
    }
    
    if(chr == 's'){
      getTemp();

      //Send one long string terminated by null
      String data = "{ \"id\": " + String(id) + ", \"temp\": " + String(temp) + " }\n";
//      data.toCharArray(dataSend,30);
 //     XBee.write(dataSend);
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
