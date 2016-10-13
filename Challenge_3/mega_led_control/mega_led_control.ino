//#include <SoftwareSerial.h>

int redPins[3] = {13, 10, 7};
int greenPins[3] = {12, 9, 6};
int bluePins[3] = {11, 8, 5};
/*int redPins[1] = {11};
int greenPins[1] = {10};
int bluePins[1] = {9};*/
int numPins = 3;
// 1 is off, 2 is on
char led1[4] = ""; // on/off, R, G, B
char led2[4] = "";
char led3[4] = "";
/*char led1_arr[4] = "";
char led2_arr[4] = "";
char led3_arr[4] = "";*/


//SoftwareSerial Serial1(2, 3); // RX, TX
bool start;
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
  Serial1.begin(9600);
  Serial.begin(9600);
  for ( int i = 0; i < numPins; i++) {
    pinMode(redPins[i], OUTPUT);
    pinMode(greenPins[i], OUTPUT);
    pinMode(bluePins[i], OUTPUT);
  }
}



void init_status() {
  char first_byte = 255; //which LED
  char second_byte = 255;   // R
  char third_byte = 255;    // G
  char fourth_byte = 255;   // B

  Serial.println("DATA: ");
  sprintf(data,  "%c%c%c%c", first_byte, second_byte, third_byte, fourth_byte);
  Serial.println(data);
  Serial1.write(data);
  //start = true;
  delay(2000);
  //Check if the server is sending data
  bool server_running = false;
  
  if (Serial1.available() > 0) {
    server_running = true;
    //Serial.println("Server");
  }

  while (server_running) {
    //Serial.println("Server running");
    if (Serial1.available() > 0) {
      Serial.println("Server_Running is true. Starting to read the Serial1.");
      uint32_t byteRead = Serial1.read();
      //Serial.println(byteRead);
      command[count] = byteRead;
      Serial.println(command[count]);
      count++;
      if (count == 4) {
        count = 0;
        int led_to_turn_on;
        Serial.print("Command[2]: ");
        int id = command[0] - 1;
        set_color(command[0] - 1, command[1], command[2], command[3]);  // red
        //break;
      }
      
    }
    
  }
  //start = true;
}
void set_color(int id, int red, int green, int blue) {
  int R = id + 1;
  char led_arr[4] = "";
  char char_id = id + 1;
  char char_red = red;
  char char_green = green+1;
  char char_blue = blue+1;
  switch(id) {
    case 0: 
    Serial.println(id);
      
      sprintf(led_arr,  "%c%c%c%c", led1[0], led1[1], led1[2], led1[3]);
      sprintf(led_arr,  "%c%c%c%c", char_id, char_red, char_green, char_blue);
      Serial.print("led: ");
      Serial.println(led_arr);
      Serial1.write(led_arr);
      //delay(2000);
      break;
    case 1: 
      
      sprintf(led_arr,  "%c%c%c%c", char_id, char_red, char_green, char_blue);
      Serial.print("led: ");
      Serial.println(led_arr);
      Serial1.write(led_arr);
      //delay(2000);
      break;
    case 2: 
      sprintf(led_arr,  "%c%c%c%c", char_id, char_red, char_green, char_blue);
      Serial.print("led: ");
      Serial.println(led_arr);
      Serial1.write(led_arr);
     // delay(2000);
      break;
  }
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;

  analogWrite(redPins[id], red);
  analogWrite(greenPins[id], green);
  analogWrite(bluePins[id], blue);
}
void loop() {
  // put your main code here, to run repeatedly:
  // ledID R G B
  //if(Serial.read() == '*'){
  while (!start) {
    Serial.println("Parsing the command sent");
    delay(2000);
    init_status();
  }
  delay(1000);
}
