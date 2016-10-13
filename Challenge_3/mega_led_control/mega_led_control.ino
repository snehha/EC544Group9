//#include <SoftwareSerial.h>

int numPins = 3;
int redPins[3] = {13, 10, 7};
int greenPins[3] = {12, 9, 6};
int bluePins[3] = {11, 8, 5};

int numBytes = 4;
byte *data = malloc(4);
byte command[4] = {0, 0, 0, 0};
byte ledStatuses[3][4] = {{1, 0, 0, 0},
  {2, 0, 0, 0},
  {3, 0, 0, 0}
};

bool initialized;
bool dataGood;

int count = 0;

void setArray(byte * data, byte a, byte b, byte c, byte d) {
  data[0] = a;
  data[1] = b;
  data[2] = c;
  data[3] = d;
}

void setup() {
  initialized = false;
  dataGood = false;
  Serial1.begin(9600);
  Serial.begin(9600);
  for ( int i = 0; i < numPins; i++) {
    pinMode(redPins[i], OUTPUT);
    pinMode(greenPins[i], OUTPUT);
    pinMode(bluePins[i], OUTPUT);
  }
}

void init_status() {
  //Send all ones to get the data in the server
  byte initializedVal = 255;
  setArray(data, initializedVal, initializedVal, initializedVal, initializedVal);
  Serial1.write(data, numBytes);
  //Wait to hear back from server
  delay(2000);

  //Check if the server is sending data and if it isnt then the statements above will be repeated every n seconds
  bool server_running = false;
  if (Serial1.available() > 0) {
    server_running = true;
    Serial.println("Server is currently Sending so going into while");
  }
  int dataCount = 0;
  while (server_running) {
    Serial.println("While server Running");

    while (Serial1.available() > 0) {
      Serial.println("There is data to read");
      char byteRead = Serial1.read();
      Serial.println(int(byteRead));
      command[count] = byteRead;
      count++;
      
      //Read 4 bytes at a time
      if (count == 4) {
        
        //If sync string read, then send 12 bytes in a row
        if (((int*)command)[0] == -1) {
          Serial.println("--------Read sync---------");
          //Send current LED Values
          for (int i = 0; i < 3; i++) {
            setArray(data, ledStatuses[i][0], ledStatuses[i][1], ledStatuses[i][2], ledStatuses[i][3]);
            //sprintf(value,"%c%c%c%c",ledStatuses[i][0],ledStatuses[i][1],ledStatuses[i][2],ledStatuses[i][3]);
            Serial1.write(data, numBytes);
          } 
          return;
        }

        else if (((int*)command)[0] == 0){
          dataGood = true;
          Serial.println("---------Read Zeroes----------");
          count=0;
          continue;
        }

        //Else if all zeroes are read then sed the led ledStatuses to match server, set colors, and then set initialized to true and break out
        if (dataGood){
          Serial.println("------Goooooooood--------");
          dataCount++;
          byte char_id = command[0];
          byte char_red = command[1];
          byte char_green = command[2];
          byte char_blue = command[3];
          setArray(ledStatuses[dataCount], char_id, char_red, char_green, char_blue);
          set_color(char_id, char_red, char_green, char_blue);
          Serial1.write(ledStatuses[dataCount],4);
          if(dataCount==3){
            initialized = true;
            dataGood = false;
            count = 0;
            return;
          }
        }
        count = 0;
      }//4 bytes read
    }//Byte is read
  }//Server is running
}//Init loop that keeps running until it hears from server

//Write to analog outputs
void set_color(int id, int red, int green, int blue) {
  uint8_t redIn = 255 - uint8_t(red);
  uint8_t greenIn = 255 - uint8_t(green);
  uint8_t blueIn = 255 - uint8_t(blue);
  Serial.println("Colors");
  Serial.print(id);
  Serial.print(redIn);
  Serial.print(greenIn);
  Serial.print(blueIn);

  analogWrite(redPins[id-1], redIn);
  analogWrite(greenPins[id-1], greenIn);
  analogWrite(bluePins[id-1], blueIn);
}

void loop() {
  // Initialize once and then just listen for client changes
  while (!initialized) {
    delay(3000);
    Serial.println("Entering Init Now");
    init_status();
  }

  //Listen for data
  //If server restarts or asks for status of LEDs, set initialized to false to initialize again
  while(Serial1.available() > 0){
    char byteRead = Serial1.read();
    Serial.println(int(byteRead));
    command[count] = byteRead;
    count++;
    if(count == 4){
      //if(((int*)command)[0] == -1){
        //Serial.println("Server wants init");
        //initialized = false;
        //break;
      //}
      //else{
        set_color(command[0],command[1],command[2],command[3]);
      //}
      count = 0;
    }
  }
}
