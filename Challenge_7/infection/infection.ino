#define redLED 4    // infected
#define greenLED 5  // not infected
#define blueLED 6   // leader
#define button 8 

bool leader = true;
bool infected = false;

int buttonState = LOW;      // current reading of button
int lastButtonState = LOW;  // the previous reading from the input pin
long lastDebounceTime = 0;  // the last time button was toggled
long debounceDelay = 50;    // the debounce time, increase if output flickers

void setup() {
  Serial.begin(9600);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(button, INPUT);
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
void clearInfection() {
  if(!leader) return; // non-leaders cannot send clear infection message
}

// non-leader sends an infection message
void spreadInfection() {
  if(leader) return;  // leaders cannot spread infection
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
        if(leader) clearInfection();  // leader sends clear infection message
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

void loop() {
  checkButtonInput();
}
