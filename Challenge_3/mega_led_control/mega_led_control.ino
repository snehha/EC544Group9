//#include <SoftwareSerial.h>

int numPins = 3;
int redPins[3] = {13, 10, 7};
int greenPins[3] = {12, 9, 6};
int bluePins[3] = {11, 8, 5};

int numBytes = 4;
byte *data = malloc(4);
byte command[4] = {0, 0, 0, 0};
byte ledStatuses[3][4] = {{1, 255, 255, 255},
  {2, 255, 255, 255},
  {3, 255, 255, 255}
};

//Used to track 4 bytes of 0s to signal that the data following is fresh
bool dataGood = false;

//Global counter for counting bytes read from one read to the next
int count = 0;
//Counter to read 3 consecutive bulb values in a row representing status
int dataCount = 0;

//Set 4 bytes of data to a,b,c,d
void setArray(byte * data, byte a, byte b, byte c, byte d) {
  data[0] = a;
  data[1] = b;
  data[2] = c;
  data[3] = d;
}

void setup() {
  dataGood = false;
  dataCount = 0;
  count = 0;
  Serial1.begin(9600);
  Serial.begin(9600);
  for ( int i = 0; i < numPins; i++) {
    pinMode(redPins[i], OUTPUT);
    pinMode(greenPins[i], OUTPUT);
    pinMode(bluePins[i], OUTPUT);
  }
}

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
  //Listen for data
  //If server restarts or asks for status of LEDs, set initialized to false to initialize again
  while (Serial1.available() > 0) {
    //Read and print
    char byteRead = Serial1.read();
    Serial.write("In an iteration, Read: ");
    Serial.println(int(byteRead));
    
    //Store bytes into char array
    command[count] = byteRead;
    count++;
    
    //Read 4 bytes at a time
    if (count == 4) {
      count = 0;
      //If sync string read, then send 12 bytes in a row which is the bulb number and status
      if (((int*)command)[0] == -1) {
        Serial.println("--------Read all 1s---------");
        //Send current LED Values
        for (int i = 0; i < 3; i++) {
          setArray(data, ledStatuses[i][0], ledStatuses[i][1], ledStatuses[i][2], ledStatuses[i][3]);
          Serial.write("Sent data :");
          Serial1.write(data, numBytes);
        } 
        continue;
      }

      /*else if (((int*)command)[0] == 0){
        dataGood = true;
        Serial.println("---------Read Zeroes so data is fresh----------");
        continue;
      }*/

      //Else if all zeroes are read then set the led ledStatuses to match server, set colors, and then set initialized to true and break out
      else{
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
          dataGood = false;
          dataCount = 0;
          continue;
        }
      }
    }//4 bytes read
  }//Byte is read
}
