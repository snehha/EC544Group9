#include <Event.h>
#include <Timer.h>
#include <SoftwareSerial.h>

#define redLED 4    // infected
#define greenLED 6  // not infected
#define blueLED 5   // leader
#define button 8 

#define ELECTION_TIMEOUT 3
#define LEADER_CHECK_COUNT 5
#define LEADER_CHECK_TIME 2000
#define MAX_UID_SIZE 65535
//Append UID to the election_start/end

SoftwareSerial XBee(2, 3); // RX, TX

bool leader_set;
bool leader;
bool leader_alive;
uint16_t uid;
uint16_t leader_uid;
uint16_t previous_uid;
byte *message_send = malloc(8);
byte *message_receive = malloc(4);
int leader_check_count = 0;
int election_count = 0;
Timer t;

void setup() {
  
  XBee.begin(9600);
  Serial.begin(9600);

  //Uid setup
  randomSeed(analogRead(0));
  uid = random(1, MAX_UID_SIZE);
  Serial.println(uid);

  //Flags
  leader = true;
  leader_set = false;
  leader_alive = false;
  leader_uid = 0;
  previous_uid = 0;
  clearArray(message_send,8);
  clearArray(message_receive,4);

  int tickEvent = t.every(LEADER_CHECK_TIME,statusTimer);

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(button, INPUT);
  
}

// lights LED states for leader, infected, not infected
void lightLED() {
  if(leader) {
    digitalWrite(blueLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
    Serial.println("Leader");
  }
  if(!leader) {
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    Serial.println("Not Leader");
  }
}

void loop() {
  //Leader_set will only be set true in runElection
  if(leader_set)
    listenOthers();
    
  //Update Timer
  t.update();
}

void statusTimer(){
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
  if(leader_set){
    lightLED();
    //Run if this Arduino is the leader
    if(leader){
      //Sent status
      //Serial.println("I'm the leader");
      writeXBee(message_send,uid,1,1);
    }
    //Run if this is a client node Arduino
    else{
      //If the leader is still false, count up to the LEADER_CHECK_COUNT
      if(!leader_alive){
        //if the LEADER_CHECK_COUNT == leader timer check run, leader dead, set the leader_set to false
        if(++leader_check_count == LEADER_CHECK_COUNT)
          leader_set = false;
      }
      leader_alive = false;
    }
  }
  else
    runElection();
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
    Serial.print("\tUID: ");
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

void clearArray(byte* message, int count){
  for(int i = 0; i < count; i++)
    message[count] = '\0';
}


void writeXBee(byte* message,uint16_t uid, byte infectionStatus, byte leaderStatus){
  preamMes(message, uid, infectionStatus, leaderStatus);
  XBee.write((char*)message);
  Serial.println("+++++++ Sending Data Log +++++++");
  printMessage(message,8);
}


unsigned int readXBee(){
  clearArray(message_receive,4);

  //Count four bytes and set xbeeAvailable to true
  int counter = 0;
  bool xbeeAvailable = false;

  //Count four -1 and set data_clean to true
  int count_ones = 0;
  bool data_clean = false;

  //ASSUMING packet is recieved with four bytes together.
  while(XBee.available()){
    char readByte = XBee.read();
    Serial.print(int(readByte));
    Serial.print(" ");
    if ((readByte == -1) && !data_clean){
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
      else{
        xbeeAvailable = true;
        data_clean = false;
        Serial.println("Printing");
        break;
      }
    }
  }
  if (xbeeAvailable) {
    Serial.println("****** Received Data Log ********");
    printMessage(message_receive,4);
    unsigned int receivedData = (byte)(message_receive[0]);
    receivedData <<= 8;
    receivedData |= (int)message_receive[1];
    receivedData <<= 8;
    receivedData |= (int)message_receive[2];
    receivedData <<= 8;
    receivedData |= (int)message_receive[3];
    return receivedData;
  }
  else{
    //Serial.println("No data received");
    return 0;
  }
}


/*************During Election**************/
void runElection(){
  
    Serial.println("Running Election");
    //Change to four bytes
    if(!leader_set)
      leader_uid = 0;
    if(leader_uid < uid){
      Serial.print("I'm running for prez with: ");
      Serial.println(uid);
      //Change to four bytes
      writeXBee(message_send,uid,1,1);
      previous_uid = uid;
    }

    //****** PROCESS THE TEXT **********//
    readElection();
}
void readElection(){
  //Recieves possible bully message from all other nodes
  //if read higher id stay silent, otherwise broadcast "leader"

  while(1){
    Serial.println("---------Reading 8 bytes if available---------");
    unsigned int receivedData = readXBee();
    uint16_t received_uid = (uint16_t)(receivedData >> 16);
    byte received_leader = (byte)(receivedData >> 8);
    byte received_cmd = (byte)receivedData;

    if(!receivedData){
      if(previous_uid == uid){
        Serial.print("No read and alone -- UID: ");
        Serial.println(previous_uid);
        if(++election_count == ELECTION_TIMEOUT){
          leader_set = true;
        }
      }
      break;
    }
    //****** IF receivedID is ZERO don't do this below **********//
    
    if(received_uid < uid){
      leader_uid = uid;
      Serial.print("Im still in it: ");
      Serial.println(uid);
      //Change to four bytes
      writeXBee(message_send,uid,1,1);
    }
    else if(received_uid > leader_uid){
      leader_uid = received_uid;
      election_count = 0;
      Serial.print("Lost. Received Highest uid: ");
      Serial.println(leader_uid);
    }
    if(previous_uid == received_uid){
      Serial.print("Previous UID: ");
      Serial.println(previous_uid);
      if(++election_count == ELECTION_TIMEOUT){
        leader_set = true;
      }
    }
    
    previous_uid = received_uid;
  }
}
/****************************************/

/**************After Election***************/
//Listen for leader is alive - Leader will send alive message periodically


int listenOthers(){

  while(1){
    //4 bytes received with 2 bytes ID, 1 byte leader, 1 byte cmd
    int receivedData = readXBee();
    uint16_t received_uid = (uint16_t)(receivedData >> 16);
    byte received_leader = (byte)(receivedData >> 8);
    byte received_cmd = (byte)receivedData;
  
    if(!receivedData){
      break;
    }    
    //Received Leader data
    if(received_uid == leader){
      //Make sure it is leader for debugging by getting ID
      if(received_leader == 1){
        leader_check_count = 0;
        leader_alive = true;
        //Command from leader
        if(received_cmd==1)
          //return 1;
          //RUN CODE FOR CLEAR
          continue;
      }
     }
    //Received Node data  
    else if(received_leader==0)
      //Infect
      if(received_cmd==2)
        continue;
        //return 2;
        //RUN CODE FOR INFECTION
  }
  return 0;
}
/******************************************/
