#include <SoftwareSerial.h>

int redPins[3] = {13, 10, 7};
int greenPins[3] = {12, 9, 6};
int bluePins[3] = {11, 8, 5};
int numPins = 3;

SoftwareSerial XBee(2, 3); // RX, TX
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
  XBee.begin(9600);
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
  XBee.write(data);
  //start = true;
  delay(2000);
  //Check if the server is sending data
  bool server_running = false;
  if (XBee.available() > 0) {
    server_running = true;
  }

  while (server_running) {
    if (XBee.available() > 0) {
      //Serial.println("Server_Running is true. Starting to read the XBee.");
      uint32_t byteRead = XBee.read();
      Serial.println(byteRead);
      command[count] = byteRead;
      Serial.println(command[count]);
      count++;
      if (count == 4) {
        count = 0;
        int led_to_turn_on;
        Serial.print("Command[2]: ");
        Serial.println(command[2]);
        set_color(command[0], command[1], command[2], command[3]);  // red
      }
      start = true;
    }
  }
}
void set_color(int id, int red, int green, int blue) {
#ifdef COMMON_ANODE
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
#endif
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
