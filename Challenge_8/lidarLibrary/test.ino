// This #include statement was automatically added by the Particle IDE.
#include "LIDARLite.h"

String SSID;
String wifiData;

String BSSID = "";

Thread* wifiThread;
Thread* servoThread;

Servo myservo;
int pos = 0;

String ignoreWifiName = "Group9";

void setup() {
    //Particle.variable("SSID", &SSID, STRING);
    Particle.variable("wifiData", &wifiData, STRING);

    myservo.attach(D3);

    wifiThread = new Thread("sample", scanWifi);
    //servoThread = new Thread("sample", moveServo);

    //ignoreWifiName = WiFi.SSID();
}

void moveServo() {
    while(true) {
        for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
            // in steps of 1 degree
            myservo.write(pos);               // tell servo to go to position in variable 'pos'
            delay(7);                         // waits 15ms for the servo to reach the position
        }
        delay(7);
        for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
            myservo.write(pos);               // tell servo to go to position in variable 'pos'
            delay(7);                         // waits 7ms for the servo to reach the position
        }
    }
}
/*
void scanWifi() {
    while(true) {
        //wifiData = "";
        String data = "";

        WiFiAccessPoint aps[20];
        int found = WiFi.scan(aps, 20);

        for (int i=0; i<found; i++) {
            BSSID = "";
            WiFiAccessPoint& ap = aps[i];

            if( ap.ssid != ignoreWifiName) {

                // BSSID string
                for(int j = 0; j < 6; j++) BSSID += String(ap.bssid[j]);

                SSID = ap.ssid;
                data += SSID + "," + BSSID + "," + String(ap.rssi) + "\n";
            }
        }
        wifiData = data;
        delay(1500);
    }
}
*/
void loop() {

    //scanWifi();
    //moveServo();
}
