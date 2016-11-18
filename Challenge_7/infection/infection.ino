#include <SoftwareSerial.h>

#define redLED 4    // infected
#define greenLED 5  // not infected
#define blueLED 6   // leader
#define button 8 

#define MAX_UID_SIZE 65535

SoftwareSerial XBee(2, 3); // RX, TX

bool leader = false;
bool infected = false;

uint16_t uid;
uint16_t leader_uid;
byte *message = malloc(4);
int leaderCheckCount = 0;
bool leaderAlive = true;

void printMessage() {
  for(int i = 0; i < 4; i++)
    Serial.print(message[i]);
   Serial.println();
}
void uidArray(byte *message, uint16_t uid, int infectionMsg, int isLeaderAlive){
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
  uidArray(message, uid, 1, leaderAlive);
  XBee.write((char*)message);
}

// non-leader sends an infection message
void spreadInfection() {
  if(leader) return;  // leader cannot send infection message
  uidArray(message, uid, 2, leaderAlive);
  XBee.write((char*)message);
  Serial.print("Sending infection message: ");
  printMessage();
}

// non-leader receives an infection message
void infectionReceived() {
  if(leader) return;  // leaders cannot get infected
  
  infected = true;
  lightLED();
}

void clearReceived() {
  // non leader receives a clear infection message
  infected = false;
  lightLED();
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
        if(leader) sendClearInfection();  // leader sends clear infection message
        else {                        
          infected = true;            // non-leader is infected
          spreadInfection();          // non-leader sends infection message
        }

        lightLED(); // change state of LEDs accordingly
      }
    }
  }
  lastButtonState = reading;
}

uint16_t readXBee() {
  int counter = 0;
  char messageRead[4];

  bool xbeeAvailable = false;

  while(XBee.available()) {
    xbeeAvailable = true;
    // message: uid doNothing(0)/clear(1)/send(2)Infection
    messageRead[counter] = XBee.read();
    if(counter++ == 3) {
      break;
    }
  }
  if(xbeeAvailable) {
    Serial.println("Message received: "); 
    printMessage();
    if(messageRead[3] == 1) {
      clearReceived();
      sendClearInfection();
      Serial.println("Clear infection message received.");

    }
    else if(messageRead[3] == 2) {
      infectionReceived();
      spreadInfection();
      Serial.println("Send infection Message received.");
    }
  
    return 0; // not sure what to return here
  }
  else {
    return 0;
  }
}

void loop() {
  checkButtonInput();
  readXBee();
}
