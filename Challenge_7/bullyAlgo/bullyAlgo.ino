#include <SoftwareSerial.h>
#include "Timer.h"

#define ELECTION_TIMEOUT 5
#define ELECTION_RESEND 3
#define LEADER_CHECK 10
#define LEADER_CHECK_TIME 1000
#define MAX_UID_SIZE 65535
//Append UID to the election_start/end

SoftwareSerial XBee(2, 3); // RX, TX

bool leader_set;
bool leader;
uint16_t uid;
uint16_t leader_uid;
byte *message = malloc(2);
int leaderCheckCount = 0;
int electionCount = 0;
int electionCountMax = 4;
Timer t;

void uidArray(byte *message, uint16_t uid){
  message[0] = (byte)(uid >> 8);
  message[1] = (byte)uid;
}

void setup() {
  
  XBee.begin(9600);
  Serial.begin(9600);
  leader = true;
  leader_set = false;
  leader_uid = 0;
  randomSeed(analogRead(0));
  uid = random(1, MAX_UID_SIZE);
  Serial.println(uid);
  int tickEvent = t.every(LEADER_CHECK_TIME,statusTimer);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(leader_set){
    runElection();
  }
  else{
    listenOthers();
    //listenButton();
  }
}

void statusTimer(){
  //Check for leader while election is not going on
  if(leader_set){
    if(leader){
      //Sent status
      
    }
    else{
      //Check previous status bit
      if(){
        
      }
    }
  }
}

unsigned int readXBee(){
  
  int counter = 0;
  char messageRead[4];
  bool xbeeAvailable = false;

  //ASSUMING packet is recieved with four bytes together.
  while(XBee.available()){
    xbeeAvailable = true;
    messageRead[counter] = XBee.read();
    if (counter++ == 3){
      break;
    }
  }
  if (xbeeAvailable){
    unsigned int receivedData = (byte)(messageRead[0]);
    receivedData <<= 8;
    receivedData |= (int)messageRead[1];
    receivedData <<= 8;
    receivedData |= (int)messageRead[2];
    receivedData <<= 8;
    receivedData |= (int)messageRead[3];
  
    return receivedData;
  }
  else{
    return 0;
  }
  
}

/**************After Election***************/
//Listen for leader is alive - Leader will send alive message periodically
int listenOthers(){

  //4 bytes received with 2 bytes ID, 1 byte leader, 1 byte cmd
  int receivedData = readXBee();
  if(!receivedData)
    return 0;
  uint16_t temp_uid = (uint16_t)(receivedData >> 16);
  byte temp_leader = (byte)(receivedData >> 8);
  byte temp_cmd = (byte)receivedData;
  
  //Received Leader data
  if(temp_uid == leader)
    //Make sure it is leader for debugging by getting ID
    if(temp_leader == 1){
      leaderCheckCount = 0;
      //Command from leader
      if(temp_cmd==1)
        return 1;
    }
  //Received Node data  
  else if(temp_leader==0)
    //Infect
    if(temp_cmd==2)
      return 2;
    
  if(leaderCheckCount == LEADER_CHECK){
    leader_uid = 0;
  }
  
  return 0;
}
/******************************************/

/*************During Election**************/
void runElection(){
  
    Serial.println("Running Election");

    //Change to four bytes
    uidArray(message, uid);
    XBee.write((char*)message);

    //****** PROCESS THE TEXT **********//
    readElection();
    
    delay(ELECTION_RESEND);
}
void readElection(){
  //Recieves possible bully message from all other nodes
  //if read higher id stay silent, otherwise broadcast "leader"

  unsigned int receivedData = readXBee();

  //****** IF receivedID is ZERO don't do this below **********//
  
  if((uint16_t)(receivedData >> 16) > uid){
    leader_uid = (uint16_t)(receivedData >> 16);
    Serial.print("My new leader is: ");
    Serial.println(leader_uid);
    //Change to four bytes
    uidArray(message, leader_uid);
    XBee.write((char*)message);
  }
}
/****************************************/

