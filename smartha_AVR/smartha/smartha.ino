#include  <TimerOne.h>          // Avaiable from http://www.arduino.cc/playground/Code/Timer1
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "IRLremote.h"

#define RX_PIN 6
#define TX_PIN 5
#define WARN_PIN 10
#define IR_PIN 3
#define RELAY1_PIN 8
#define RELAY2_PIN 9
#define BUTTON_PIN 14

SoftwareSerial Serial1(RX_PIN, TX_PIN);


volatile int i = 0;             // Variable to use as a counter volatile as it is in an interrupt
volatile boolean zero_cross = 0; // Boolean to store a "switch" to tell us if we have crossed zero
int AC_pin = 11;                // Output to Opto Triac
int dim = 100;                    // Dimming level (0-128)  0 = on, 128 = 0ff
int inc = 1;                    // counting up or down, 1=up, -1=down
int freqStep = 75;    // This is the delay-per-brightness step in microseconds.


byte sw_1 = EEPROM.read(1);
byte sw_2 = EEPROM.read(2);
byte sw_p = EEPROM.read(3);
byte sw_n = EEPROM.read(4);

CNec IRLremote;

bool r1 = true;
bool r2 = true ;
bool but = false;

volatile byte x = 0;
int valueCounter = 0;
int nowWhat = 0;

void setup()
{
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(WARN_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  delay(3000);
  Serial.begin(115200);
  Serial1.begin(9600); //Baud rate for communicating with ESP8266. Your's might be different.
  IRLremote.begin(IR_PIN);

  pinMode(AC_pin, OUTPUT);
  pinMode(2, INPUT_PULLUP); // Set the Triac pin as output
  attachInterrupt(0, zero_cross_detect, FALLING);    // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
  Timer1.initialize(freqStep);                      // Initialize TimerOne library for the freq we need
  Timer1.attachInterrupt(dim_check, freqStep);
}

void loop()
{
  checkButton();
  recieveWiFi();
  recieveIR();
  updateRelays();
}

void zero_cross_detect() {
  zero_cross = true;               // set the boolean to true to tell our dimming function that a zero cross has occured
  i = 0;
  digitalWrite(AC_pin, LOW);       // turn off TRIAC (and AC)
}

// Turn on the TRIAC at the appropriate time
void dim_check() {
  if (zero_cross == true) {
    if (i >= dim) {
      digitalWrite(AC_pin, HIGH); // turn on light
      i = 0; // reset time step counter
      zero_cross = false; //reset zero cross detection
    }
    else {
      i++; // increment time step counter
    }
  }
}

void updateRelays() {
  digitalWrite(RELAY1_PIN, r1);
  digitalWrite(RELAY2_PIN, r2);
}

void beep() {
  tone(WARN_PIN, 2000, 50);
}

void doublePeep() {
  beep();
  delay(100);
  beep();
}

void recieveWiFi() {
  if (Serial1.available()) {
    delay(10);
    String data = Serial1.readStringUntil('\r');

    if (data.indexOf(F("1")) != -1) {
      r1 = !r1;
      beep();
    }

    if (data.indexOf(F("2")) != -1) {
      r2 = !r2;
      beep();
    }

    if (data.indexOf(F("3")) != -1) {
      dim = dim + 16;
      beep();
    }

    if (data.indexOf(F("4")) != -1) {
      dim = dim - 16;
      beep();
    }
  }
}

void checkButton() {
  but = digitalRead(BUTTON_PIN);
  delay(200);
}

void recieveIR() {
  if (IRLremote.available())
  {

    delay(200);



    auto data = IRLremote.read();

    if (data.command == sw_1) {
      r1 = !r1;
      beep();
    }
      else if (data.command == sw_2) {
        r2 = !r2;
        beep();
      }
      else if (data.command == sw_p) {
        dim = dim + 16;
        beep();
      }
      else if (data.command == sw_n) {
        dim = dim - 16;
        beep();
      }
      else {
         if (!but){
        delay(200);
        switch (valueCounter) {
          case 0:
            sw_1 = data.command;
            EEPROM.write(1, data.command);
            doublePeep();
            valueCounter = 1;
            break;
          case 1:
            sw_2 = data.command;
          EEPROM.write(2, data.command);
          doublePeep();
          valueCounter = 2;
            break;
        }
        }

        //        if (valueCounter == 2) {
        //          sw_p = data.command;
        //          EEPROM.write(3, data.command);
        //          doublePeep();
        //          valueCounter = 3 ;
        //        }
        //        if (valueCounter == 3) {
        //          sw_n = data.command;
        //          EEPROM.write(4, data.command);
        //          doublePeep();
        //          valueCounter = 0;
        //          but = false;
        //        }

      }
    }
  }
