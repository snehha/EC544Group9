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
char temp[10] = "";
//Reading one char at a time
char chr;
//Support for 1,000 devices at most
char id[3] = "";
char randBuff[4] = "";
char dataSend[4] = "";
int count;
int counter;
///FF+rand_num is request to join
///////////////////////////////////////////////////


//////////////////////////////////////////////////
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
  randomSeed(analogRead(1));
  pinMode(13,OUTPUT);
  join = false;
}

void initial_join() {
  Serial.write("Init\n");
  digitalWrite(13,1);
  chr = '\0';

  clearArr(id,3);
  clearArr(randBuff,4);
  clearArr(dataSend,30);
  clearArr(temp,10);
  count = 0;
  counter = 0;
  
  //Generate Random Number
  randNum = random(10000);
  Serial.write(randNum);

  //Ask to Join
  String data = "j" + String(randNum) + "\n";
  data.toCharArray(dataSend,8);
  XBee.write(dataSend);
  Serial.write(dataSend);
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
  int find_period;
  steinhart = log (average / THERMISTORNOMINAL);     // ln (R/Ro)
  steinhart = steinhart/BCOEFFICIENT;               // ln(R/Ro)/B
  steinhart += (1.0 / (TEMPERATURENOMINAL + 273.15)); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart =  steinhart - 273.15;                         // convert to C
  f_temp = (steinhart * 9.0)/ 5.0 + 32.0;
  dtostrf(f_temp, 4, 2, temp);
  String string_temperature = String(f_temp, 2);
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
//      String data = "{ \"id\": " + String(id) + ", \"temp\": " + String(temp) + " }\n";
//      data.toCharArray(dataSend,30);
//
//      Serial.write(dataSend);
//      XBee.write(dataSend);
      //XBee.write(dataSend);
     
      //Might need to delay to prevent buffer overflow
      //delay(randNum(1000));
    }
  }
}
