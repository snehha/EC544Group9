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
//Will send 4 bytes every time -> either join or send temp/id
char dataSend[4] = "";

uint16_t firstbytes;
uint16_t secondbytes;

int count;
int counter;
uint16_t randNum;

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
  //Serial.write("Init\n");
  digitalWrite(13,1);
  chr = '\0';

  clearArr(id,2);
  clearArr(randBuff,4);
  clearArr(dataSend,4);
  clearArr(temp,2);
  count = 0;
  counter = 0;
  
  //Generate Random Number max 2^16-1
  randNum = random(pow(2,16)-1);

  //Ask to Join
  //String data = "j" + String(randNum) + "\n";
  //data.toCharArray(dataSend,8);
  char data[5];
  Serial.write("Join:65535\nRandNum:%u\n",randNum);
  sprintf(data,"%u%u\n",65535,randNum);
  Serial.write(data);
  delay(5000);
  return;
  //XBee.write(dataSend);
  //Serial.write(dataSend);
  delay(5000);
  
  bool serverRunning = false;
  if(XBee.available() > 0)
    serverRunning = true;

  //Keep reading until you get an id
  bool readId = false;
  bool readComma = false;
  
  while(serverRunning){
    digitalWrite(13,0);
    if(counter++ == 1000)
      return;
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
        Serial.write("ID");
        if(chr == '\n'){
          id[count] = '\0';
          join = true;
          Serial.write(id);
          Serial.write('\n');
          Serial.write("Joined");
          Serial.write('\n');
          return;
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
