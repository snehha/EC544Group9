#include <SoftwareSerial.h>

#define redLED 4    // infected
#define greenLED 6  // not infected
#define blueLED 5   // leader
#define button 8 

#define MAX_UID_SIZE 65535

SoftwareSerial XBee(2, 3); // RX, TX

bool leader = false;
bool infected = false;
bool immune = false;

uint16_t uid;
uint16_t leader_uid;
byte *message = (byte*) malloc(4);
char leaderAlive = '1';

void printMessage() {
  for(int i = 0; i < 4; i++) {
    Serial.print(message[i]);
    Serial.print(" ");
  }
   Serial.println();
}
void uidArray(byte *message, uint16_t uid, char infectionMsg, char isLeaderAlive){
  message[0] = (byte)(uid >> 8);
  message[1] = (byte)uid; 
  message[2] = (byte)infectionMsg;
  message[3] = (byte)isLeaderAlive;
}

int buttonState;            // current reading of button
int lastButtonState = LOW;  // the previous reading from the input pin
long lastDebounceTime = 0;  // the last time button was toggled
long debounceDelay = 50;    // the debounce time, increase if output flickers

void setup() {
  XBee.begin(9600);
  Serial.begin(9600);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(button, INPUT);
  uid = random(1, MAX_UID_SIZE);
  lightLED();
}

// lights LED states for leader, infected, not infected
void lightLED() {
  if(leader) {
    digitalWrite(blueLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
    Serial.println("Leader");
  }

  if(!leader && !infected) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(blueLED, LOW);
    digitalWrite(redLED, LOW);
    Serial.println("not infected");
  }

  if(!leader && infected) {
    digitalWrite(redLED, HIGH);
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, LOW);
    Serial.println("infected");
  }
}

// The leader sends a clear infection message
void sendClearInfection() {
   
  uidArray(message, (uint16_t)65535, '1', leaderAlive);
  Serial.print("Sending clear message: ");
  printMessage();
  XBee.write((char*)message);
}

// non-leader sends an infection message
void spreadInfection() {
  if(leader) {
    return;  // leader cannot send infection message
  }
  Serial.print("Sending infection message: ");
  printMessage();
  uidArray(message, (uint16_t)65535, '2', leaderAlive);
  XBee.write((char*)message);
}

// non-leader receives an infection message
void infectionReceived() {
  if(leader){
    return;  // leaders cannot get infected
  }
  infected = true;
  lightLED();
}

void clearReceived() {
  // non leader receives a clear infection message
  infected = false;
  lightLED();
  //delay(2000); // cannot be infected 2 seconds after clear
  immune = true;
}

// detects input of button with debouncing
void checkButtonInput() {
  int reading = digitalRead(button);  // current reading of button, debouncing

  // switch changed due to noise or pressing
  if (reading != lastButtonState) {
    lastDebounceTime = millis();      // reset the debouncing timer
  }

  // delay time for debouncing has been reached
  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != buttonState) {     // button state changed
      buttonState = reading;

      // button press was detected with debouncing taken into account
      if (buttonState == LOW) {
        Serial.println("button pressed");
        if(leader) {
          Serial.println("Sending clear.");
          sendClearInfection();  // leader sends clear infection message
        }
        else {   
          //if(infected) return;        // if already infected, cannot get another infection                     
          infected = true;            // non-leader is infected
          lightLED();
          Serial.println("Sending infection.");
          spreadInfection();          // non-leader sends infection message
          return;
        }
        lightLED(); // change state of LEDs accordingly
      }
    }
  }
  lastButtonState = reading;
}

void printReceivedMessage(char* messageRead) {
  Serial.println("Message received: "); 
    for(int i = 0; i < 4; i++) {
      Serial.print((int)messageRead[i]);
      Serial.print(" ");
    }
}

void readMessage(char* messageRead) {
   if(messageRead[2] == '0') return; // do nothing command
   if(messageRead[2] == '1') {
      Serial.println("------------------------------clear------------------------------");
      if(!infected){
        return;
      }
      Serial.println("Clear infection message received.");
      clearReceived();
      sendClearInfection();
      Serial.println("------------------------------------------------------------");
   }
   else if(messageRead[2] == '2') {
      Serial.println("------------------------------infection------------------------------");
      if(infected){ // cannot get infected again
        return;
      }
      Serial.println("Infection message received.");
      infectionReceived();
      spreadInfection();
      Serial.println("------------------------------------------------------------");
   }
}

unsigned int readXBee() {
  int counter = 0;
  char messageRead[4];
  bool xbeeAvailable = false;

  while(XBee.available()) {
    if(counter == 4){
      //read_flag = 1;
      break;
    }
    xbeeAvailable = true;
    // message: uid doNothing(0)/clear(1)/send(2)Infection
    messageRead[counter] = XBee.read();
    Serial.print("Reading Message at counter ");
    Serial.print(counter);
    Serial.print(": ");
    Serial.println((int)messageRead[counter]);
    counter++;
  }
  if(xbeeAvailable) {
    printReceivedMessage(messageRead);
    Serial.println();
    readMessage(messageRead);
    return 0; // not sure what to return here
  }
  else {
    return 0;
  }
}

void loop() {
  if(!immune){
    checkButtonInput();
  }
  else {
    delay(2000);
    immune = false;
  }
  readXBee();
}
