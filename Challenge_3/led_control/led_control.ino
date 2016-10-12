#include <Adafruit_NeoPixel.h>

#include <SoftwareSerial.h>

#define red_pin 8
#define green_pin 9
#define blue_pin 10

SoftwareSerial XBee(2, 3); // RX, TX
//Adafruit_NeoPixel s = Adafruit_NeoPixel(2, A0, NEO_GRB + NEO_KHZ800);
bool start;// = false;// = false;
char data[4] = "";
String input_string;
String led_id;
String red_color;
String green_color;
String blue_color;
uint8_t led_id_int = 0;
uint8_t red_int = 0;
uint8_t green_int = 0;
uint8_t blue_int = 0;
uint8_t byteRead;
void init_status();
int count = 0;
int command[4] = {};

void setup() {
  // put your setup code here, to run once:
  start = false;
  XBee.begin(9600);
  Serial.begin(9600);
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT); 
//  digitalWrite(red_pin, LOW);
//  digitalWrite(green_pin, LOW);
//  digitalWrite(blue_pin, LOW);
}



void init_status(){
  char first_byte = 50; //which LED
  char second_byte = 80;   // R
  char third_byte = 90;    // G
  char fourth_byte = 100;   // B
  
  Serial.println("DATA: ");
  sprintf(data,  "%c%c%c%c", first_byte, second_byte, third_byte, fourth_byte);
  Serial.println(data);
  XBee.write(data);
  //start = true;
    delay(2000);
  //Check if the server is sending data
  bool server_running = false;
//  if(XBee.available() > 0){
//      Serial.println("Starting to read the XBee.");
//      uint32_t byteRead = XBee.read();
//      Serial.write(byteRead);
//   }
  if(XBee.available() > 0){
    server_running = true;
  }  
  
  while(server_running) {
    if(XBee.available() > 0){
      //Serial.println("Server_Running is true. Starting to read the XBee.");
      uint32_t byteRead = XBee.read();

      //Serial.println(byteRead);
        command[count] = byteRead;
        Serial.println(command[count]);
        count++;
      if(count == 4) {
        count = 0;
        int led_to_turn_on;
        //digitalWrite(13, 1);
        Serial.print("Command[2]: ");
        Serial.println(command[2]);
        switch(command[0]) {
          case 1: 
            led_to_turn_on = 8;
             break;
          case 2:
            led_to_turn_on = 9;
            break;
          case 3:
            led_to_turn_on = 10;
            break;
        }
        //digitalWrite(led_to_turn_on, !digitalRead(led_to_turn_on));
//          s.begin();
//          s.setBrightness(100);
//          s.setPixelColor(led_to_turn_on,command[1],command[2],command[3]);

        set_color(command[1], command[2], command[3]);  // red
        //delay(1000);
        
      }
        /*int led_to_turn_on;
        //digitalWrite(13, 1);
        switch(command[0]) {
          case 1: 
            led_to_turn_on = 8;
             break;
          case 2:
            led_to_turn_on = 9;
            break;
          case 3:
            led_to_turn_on = 10;
            break;
        }
        if(red_int == 255){
          digitalWrite(led_to_turn_on, HIGH);
        }
        else if(red_int == 0){
          digitalWrite(led_to_turn_on, LOW);
        }*/
        start = true;
    }
    
    
  }
//    );
//    //led_id = substring(0, 8);
    
    
}
void set_color(int red, int green, int blue){
//  #ifdef COMMON_ANODE
//    red = 255 - red;
//    green = 255 - green;
//    blue = 255 - blue;
//  #endif
  Serial.print("Green pin: ");
  Serial.println(green);
  analogWrite(red_pin, red);
  analogWrite(green_pin, green);
  analogWrite(blue_pin, blue);  
}
void loop() {
  // put your main code here, to run repeatedly:
  // ledID R G B
  //if(Serial.read() == '*'){
    while(!start){ 
      Serial.println("Parsing the command sent");
      delay(2000);
      init_status();
    }
    delay(1000);
}
