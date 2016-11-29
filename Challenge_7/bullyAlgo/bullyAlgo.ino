#include <Event.h>
#include <Timer.h>
#include <SoftwareSerial.h>

#define redLED 4    // infected
#define greenLED 6  // not infected
#define blueLED 5   // leader
#define button 8 

#define ELECTION_TIMEOUT 3
#define LEADER_CHECK_COUNT 3
#define LEADER_CHECK_TIME 3000
#define MAX_UID_SIZE 65535
//Append UID to the election_start/end

SoftwareSerial XBee(2, 3); // RX, TX

bool leader_set;  //Leader has been determined
bool leader;  //I am the leader
bool leader_alive;  //Received msg from leader after previous timeout check
bool infected;
bool immune;

uint16_t uid; //My uid
uint16_t leader_uid;  //Leader uid
uint16_t previous_leader;

int send_size = 9;
int receive_size = 5;
byte *message_send = malloc(send_size); //Send 8 bytes with first 4 bytes all 1s as preamble
byte *message_receive = malloc(receive_size);  //Received 8 bytes but store 4 since the first 4 are preamble used for alligning

//Counters for leader check, election, and debugging received messages
int leader_check_count = 0;
int election_count = 0;
int xbee_avail_counter = 0;


int buttonState;            // current reading of button
int lastButtonState = LOW;  // the previous reading from the input pin
long lastDebounceTime = 0;  // the last time button was toggled
long debounceDelay = 50;    // the debounce time, increase if output flickers

//Timer to send and receive
Timer t;

void setup() {
  XBee.begin(9600);
  Serial.begin(9600);

  //Uid setup
  randomSeed(analogRead(0));
  uid = random(1, MAX_UID_SIZE);
  Serial.print("Generated Random Number: ");
  Serial.println(uid);

  //Flags
  leader = true;
  leader_set = false;
  leader_alive = false;
  infected = false;
  immune = false;
  leader_uid = uid;
  previous_leader = uid;
  clearArray(message_send,send_size);
  clearArray(message_receive,receive_size);

  int tickEvent = t.every(LEADER_CHECK_TIME,statusTimer);

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(button, INPUT);

  //Clear buffer on startup
  while(XBee.available())
    char dont_care = XBee.read();
  
}

// lights LED states for leader, infected, not infected
void lightLED() {
  if(!leader_set){
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
  }
  else{
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
}

// The leader sends a clear infection message
void sendClearInfection() {
   
  writeXBee(message_send, (uint16_t)uid, 1, 1);
  Serial.print("Sending clear message: ");
}

// non-leader sends an infection message
void spreadInfection() {
  if(leader) {
    return;  // leader cannot send infection message
  }
  Serial.print("Sending infection message: ");
  writeXBee(message_send, (uint16_t)uid,2,2);
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

void loop() {
  //Leader_set will only be set true after runElection
  if(leader_set)
    listenOthers();

  if(!immune){
    checkButtonInput();
  }
    
  //Update Timer
  t.update();
}

void clearArray(byte* message, int count){
  for(int i = 0; i < count; i++)
    message[i] = 0;
}

void printStatus(){
  Serial.println("-------------------------Entering Print Status----------------------");
  Serial.print("My Uid: ");
  Serial.println(uid);
  Serial.print("Leader uid: ");
  Serial.println(leader_uid);
  Serial.print("Leader Set: ");
  if(leader_set)
    Serial.println("True");
  else
    Serial.println("False");
  Serial.print("I am Leader: ");
  if(leader)
    Serial.println("True");
  else
    Serial.println("False");
  Serial.print("Leader Alive: ");
  if(leader_alive)
    Serial.println("True");
  else
    Serial.println("False");
  Serial.println("-------------------------Leaving Print Status-----------------------");
}

void printMessage(byte* message,int count){
  if(count == 8){
    Serial.print("Preamble: ");
    Serial.print((int)(message[0] & message[1] & message[2] & message[3]));
    Serial.print("\tUID: ");
    Serial.print((((uint16_t)(message[4])) << 8) | (uint16_t)(message[5]));
    Serial.print("\tCommand: ");
    Serial.print((uint16_t)message[6]);
    Serial.print("\tStatus: ");
    Serial.println((uint16_t)message[7]);
  }
  else{
    Serial.print("UID: ");
    Serial.print((((uint16_t)(message[0])) << 8) | (uint16_t)(message[1]));
    Serial.print("\tCommand: ");
    Serial.print((uint16_t)message[2]);
    Serial.print("\tStatus: ");
    Serial.println((uint16_t)message[3]);
  }
}


void preamMes(byte *message, uint16_t uid, byte infectionStatus, byte leaderStatus){
  message[0] = 255;
  message[1] = 255;
  message[2] = 255;
  message[3] = 255;
  message[4] = (byte)(uid >> 8);
  message[5] = (byte)uid; 
  message[6] = infectionStatus;
  message[7] = leaderStatus;
}

void writeXBee(byte* message,uint16_t uid, byte infectionStatus, byte leaderStatus){
  clearArray(message,send_size);
  preamMes(message, uid, infectionStatus, leaderStatus);
  XBee.write((char*)message);
  delay(10);
  Serial.print("Data Send----------");
  printMessage(message,8);
}


void readXBee(){
  clearArray(message_receive,receive_size);

  //Count four bytes and set xbeeAvailable to true
  int counter = 0;
  bool xbeeAvailable = false;

  //Count four -1 and set data_clean to true
  int count_ones = 0;
  bool data_clean = false;

  //ASSUMING packet is recieved with 8 bytes together.
  while(XBee.available()){
    char readByte = XBee.read();
    Serial.println(int(readByte));
    if (((byte)readByte == 255) && !data_clean){
      count_ones++;
      if(count_ones == 4){
        data_clean = true;
        counter = 0;
      }
      else
        continue;
    }
    else{
      if(counter < 4){
        message_receive[counter++] = readByte;
      }
      if(counter==4){
        xbeeAvailable = true;
        data_clean = false;
        break;
      }
    }
  }
  if (xbeeAvailable) {
    Serial.print("Data Receive-------------");
    printMessage(message_receive,4);
  }
  else{
    clearArray(message_receive,receive_size);
  }
}


/*************During Election**************/
void runElection(){
    Serial.println("*****************Running Election******************");
    if(leader_uid == uid){
      Serial.print("I'm running for prez with: ");
      Serial.println(uid);
      writeXBee(message_send,uid,1,3);
      previous_leader = uid;
    }

    //****** PROCESS THE TEXT **********//
    readElection();
}
void readElection(){
  //Recieves possible bully message from all other nodes
  //if read higher id stay silent, otherwise broadcast "leader"

  while(1){
    readXBee();
    uint16_t received_uid = ((((uint16_t)message_receive[0]) << 8) | (uint16_t)message_receive[1]);
    byte received_leader = message_receive[2];
    byte received_cmd = message_receive[3];

    //Read buffer is empty
    //Check for redundant leader messages
    if(!received_uid){
      if(previous_leader == leader_uid){
        //Once it goes in here, it will not run election and just listen untill it gets a new non leader message with leader byte set
        if(++election_count == ELECTION_TIMEOUT){
          Serial.println("ELECTION_________________________________________________OVER");
          leader_set = true;
          lightLED();
          election_count = 0;
        }
      }
      leader_uid = previous_leader;
      break;
    }

    printMessage(message_receive,4);

    //Still in it so listen
    if(received_uid <= uid && leader){
      previous_leader = uid;
      Serial.println("Received lower uid.Still Leader");
      //election_count++;
      //writeXBee(message_send,uid,1,1);
    }
    //If someone bigger, then set leader to false
    if(received_uid > leader_uid){
      Serial.print("Received Highest Leader UID: ");
      Serial.println(received_uid);
      leader = false;
      previous_leader = received_uid;
      //leader_set = true;
    }
    /*if(received_uid > leader_uid){
      leader_uid = received_uid;
      election_count++;
      leader_set = true;
      Serial.print("Received Highest uid: ");
      Serial.println(leader_uid);
    }
    if(previous_leader == received_uid){
      Serial.print("Previous UID: ");
      Serial.println(previous_leader);
      if(++election_count == ELECTION_TIMEOUT){
        leader_set = true;
        leader = true;
        Serial.println("Leader Decided");
        break;
      }
    }*/
  }
}
/****************************************/

/**************After Election***************/
//Listen for leader is alive - Leader will send alive message periodically
void listenOthers(){
  while(1){
    //4 bytes received with 2 bytes ID, 1 byte leader, 1 byte cmd
    readXBee();
    uint16_t received_uid = ((((uint16_t)message_receive[0]) << 8) | (uint16_t)message_receive[1]);
    byte received_leader = message_receive[2];
    byte received_cmd = message_receive[3];
  
    if(!received_uid){
      break;
    }    
    //Received Leader data so execute command
    if(received_uid == leader_uid){
      Serial.println("----------------------------------I HEARD THE LEADER----------------------------------");
      if(received_leader == 1){
        leader_check_count = 0;
        leader_alive = true;
        //Command from leader
        if(received_cmd==1){
          clearReceived();
        }
      }
     }
    //Someone else thinks they are leader
    else if(received_leader==1){
      if(leader){
        if(received_uid > uid){
          leader = false;
          leader_set = false;
          lightLED();
        }
      }
    }
    //Node wants to infect
    else if(received_cmd==2){
      infectionReceived();
      continue;
    }
  }
}
/******************************************/

void statusTimer(){
  lightLED();
  //Check for leader while election is not going on
  /***
   *  After every LEADER_CHECK_COUNT timer runs, 
   *  IF the leaderAlive is still false, then the leader is dead.
   *  
   *  ELSE then the leader is assumed to still be alive.
   *  
   *  The leaderAlive is set by the listenOthers() function, which runs
   *  on the loop, it will read all messages but everytime it seems the
   *  correct leader, it will flip/keep the leaderAlive bool bit true.
   ***/
  Serial.println("++++++++++++++++++++++++Before Leader Check+++++++++++++++++++++++++");
  printStatus();
  //Will be false whenever it receives uid that is not leader with leader byte set, which is initially sent by all
  if(leader_set){
    //Run if this Arduino is the leader
    if(leader){
      //Sent status
      Serial.println("I'm the leader");
      writeXBee(message_send,uid,1,3);
    }
    //Run if this is a client node Arduino
    else{
      if(immune)
        immune = false;
      else if(infected)
        spreadInfection();
      //If the leader is still false, count up to the LEADER_CHECK_COUNT
      if(!leader_alive){
        //if the LEADER_CHECK_COUNT == leader timer check run, leader dead, set the leader_set to false
        if(++leader_check_count == LEADER_CHECK_COUNT){
          leader_uid = 0;
          previous_leader = uid;
          leader_set = false;
          leader = true;
        }
      }
      leader_alive = false;
    }
  }
  else
    runElection();
  printStatus();
  Serial.println("++++++++++++++++++++++++After Leader Check+++++++++++++++++++++++++");
}
