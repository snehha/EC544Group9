#include <SoftwareSerial.h>

#define ELECTION_TIMEOUT 5
#define ELECTION_RESEND 3
#define LEADER_CHECK 10
#define MAX_UID_SIZE 65535
//Append UID to the election_start/end

SoftwareSerial XBee(2, 3); // RX, TX

bool leader;
uint16_t uid;
uint16_t leader_uid;
byte *message = malloc(2);
int leaderCheckCount = 0;
void uidArray(byte *message, uint16_t uid, byte infectionStatus, byte leaderStatus){
  message[0] = (byte)(uid >> 8);
  message[1] = (byte)uid; 
  message[2] = infectionStatus;
  message[3] = leaderStatus;
}

void setup() {
  
  XBee.begin(9600);
  Serial.begin(9600);
  leader = true;
  leader_uid = 0;
  randomSeed(analogRead(0));
  uid = random(1, MAX_UID_SIZE);
  Serial.println(uid);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(!leader_uid){
    startElection();
  }
  else{
    listenOthers();
  }

}

uint16_t readXBee(){

  int counter = 0;
  char messageRead[2];

  bool xbeeAvailable = false;

  //ASSUMING packet is recieved with two byte together.
  while(XBee.available()){
    xbeeAvailable = true;
    messageRead[counter] = XBee.read();
    if (counter++ == 1){
      break;
    }
  }
  if (xbeeAvailable){
    uint16_t receivedID = (byte)(messageRead[0]);
    receivedID = receivedID << 8;
    receivedID |= (uint16_t)messageRead[1];
  
    return receivedID;
  }
  else{
    return 0;
  }
  
}
void listenOthers(){
 
  if(leaderCheckCount == LEADER_CHECK){
    leader_uid = 0;
  }

  int receivedData = readXBee();
  if((uint16_t)(receivedData>>8) == leader)
    leaderCheckCount++;
  else if((byte)receivedID==0)
}

void readElection(){
  //Recieves possible bully message from all other nodes
  //if read higher id stay silent, otherwise broadcast "leader"

  uint16_t receivedID = readXBee();

  //****** IF receivedID is ZERO don't do this below **********//

  if(receivedID > uid){
    leader_uid = receivedID;
    Serial.print("My new leader is: ");
    Serial.println(leader_uid);
    uidArray(message, leader_uid);
    XBee.write((char*)message);
  }
}
void startElection(){
  
    Serial.println("Starting Election");

    //Send your UID
    uidArray(message, uid);
    XBee.write((char*)message);

    //****** PROCESS THE TEXT **********//
    readElection();
    
    delay(ELECTION_RESEND);
}

