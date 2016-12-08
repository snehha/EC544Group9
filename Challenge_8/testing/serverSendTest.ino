#include "application.h"
int led = D7;
void setup()
{
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
    Particle.function("SeverSend", serverSendTest);
}

int serverSendTest(String command)
{
    if(command == "up")
    {
        Serial.println("up");
        return 1;
    }
    else if(command == "down")
    {
        Serial.println("down");
        return 2;
    }
    else if(command == "left")
    {
        Serial.println("left");
        return 3;
    }
    else if(command == "right")
    {
        Serial.println("right");
        return 4;
    }
    else
    {
      return -1;
    }
}

void loop()
{
    digitalWrite(led, HIGH);
}
