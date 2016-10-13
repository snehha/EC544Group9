//#include <SoftwareSerial.h>

int redPins[3] = {13, 10, 7};
int greenPins[3] = {12, 9, 6};
int bluePins[3] = {11, 8, 5};
int numPins = 3;
char ledStatuses[3][4] = {{1,0,0,0},
                          {2,0,0,0},
                          {3,0,0,0}};

//SoftwareSerial Serial1(2, 3); // RX, TX
bool start;
uint8_t byteRead;
int count = 0;
char command[4] = {};

void setup() {
  // put your setup code here, to run once:
  start = false;
  Serial1.begin(9600);
  Serial.begin(9600);
  for ( int i = 0; i < numPins; i++) {
    pinMode(redPins[i], OUTPUT);
    pinMode(greenPins[i], OUTPUT);
    pinMode(bluePins[i], OUTPUT);/*
    ledStatuses[i][0] = i+1;
    ledStatuses[i][1] = -1;
    ledStatuses[i][2] = -1;
    ledStatuses[i][3] = -1;*/
  }
}



void init_status() {
  
//  for(int i= 0; i < 3; i++){
//      char value[4];
//      sprintf(value,"%c%c%c%c",ledStatuses[i][0],ledStatuses[i][1],ledStatuses[i][2],ledStatuses[i][3]);
//      Serial1.write(value);
//   }
//   Serial.println("Processed Initial Send");

  //Check if the server is sending data
  bool server_running = false;

  if (Serial1.available() > 0) {
    server_running = true;
    Serial.println("Server");
  }

  while (server_running) {
    //Serial.println("Server running");
    
    if (Serial1.available() > 0) {
      Serial.println("Server_Running is true. Starting to read the Serial1.");
      byteRead = Serial1.read();
      //Serial.println(byteRead);
      command[count] = byteRead;
      Serial.println(command[count]);
      count++;
      if (count == 4) {
        if(command[0] == -1){
          for(int i= 0; i < 3; i++){
            char value[4];
            Serial.print("Sending LED: ");
            Serial.println(int(ledStatuses[i][0]));
            Serial.println(int(ledStatuses[i][1]) + int(ledStatuses[i][2]) + int(ledStatuses[i][3])); 
            sprintf(value,"%c%c%c%c",ledStatuses[i][0],ledStatuses[i][1],ledStatuses[i][2],ledStatuses[i][3]);
            Serial1.write(value);
          }
        }
        count = 0;
        int led_to_turn_on;
        Serial.print("Command[2]: ");
        int id = command[0] - 1;
        char led_arr[4] = "";
        char char_id = command[0];
        char char_red = command[1] + 1;
        char char_green = command[2] + 1;
        char char_blue = command[3] + 1;
        sprintf(led_arr,  "%c%c%c%c", char_id, char_red, char_green, char_blue);
        for(int i = 0; i < 4; i++){
            ledStatuses[id][i] = led_arr[i];
         }
        set_color(id, command[1], command[2], command[3]);  // red
        //break;
      }
      
    }
    
  }
  //start = true;
}

void set_color(int id, int red, int green, int blue) {
  /*char led_arr[4] = "";
  char char_id = id + 1;
  char char_red = red+1;
  char char_green = green+1;
  char char_blue = blue+1;
  
  sprintf(led_arr,  "%c%c%c%c", char_id, char_red, char_green, char_blue);
  Serial1.write(led_arr);
*/
  Serial.print("Saving colors: ");
  Serial.println(uint8_t(ledStatuses[id][0]));
  Serial.println(uint8_t(ledStatuses[id][1]));
  Serial.println(uint8_t(ledStatuses[id][2]));
  Serial.println(uint8_t(ledStatuses[id][3]));
  
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
    Serial.println("Running Again");
    delay(1000);
    init_status();
  }
  delay(1000);
}
