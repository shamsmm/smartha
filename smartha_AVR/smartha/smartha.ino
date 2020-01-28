#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "IRLremote.h"

#define RX_PIN 6
#define TX_PIN 5
#define WARN_PIN 10
#define IR_PIN 3
#define RELAY1_PIN 8
#define RELAY2_PIN 9
#define TRIACZC_PIN 2
#define TRIACI_PIN 11
#define BUTTON_PIN 14

SoftwareSerial Serial1(RX_PIN, TX_PIN);

byte sw_1 = EEPROM.read(1);
byte sw_2 = EEPROM.read(2);
byte sw_p = EEPROM.read(3);
byte sw_n = EEPROM.read(4);

CNec IRLremote;

bool r1 = false;
bool r2 = false ;

volatile byte x = 0;
int valueCounter = 0;

void setup()
{
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(TRIACI_PIN, OUTPUT);
  pinMode(WARN_PIN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(TRIACZC_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  Serial1.begin(9600); //Baud rate for communicating with ESP8266. Your's might be different.

  IRLremote.begin(IR_PIN);

  attachInterrupt(digitalPinToInterrupt(TRIACZC_PIN),updateTriac,FALLING);
}

void loop()
{
  recieveWiFi();
  recieveIR();
  updateRelays();
}

void updateTriac() {
    x = map(x,0,255,7000,10);
    delayMicroseconds(x);
    digitalWrite(TRIACI_PIN, HIGH);
    delayMicroseconds(100);
    digitalWrite(TRIACI_PIN, LOW);
}

void updateRelays() {
  digitalWrite(RELAY1_PIN, r1);
  digitalWrite(RELAY2_PIN, r2);
}

void beep() {
  tone(WARN_PIN,2000,50);
}

void doublePeep() {
  beep();
  delay(50);
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
      x = x+20;
      beep();
    }

    if (data.indexOf(F("4")) != -1) {
      x= x-20;
      beep();
    }
  }
}

void recieveIR() {
  if (IRLremote.available())
  {

    delay(100);

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
      x = x+80;
      beep();
    }
    else if (data.command == sw_n) {
      x = x-80;
      beep();
    }
    else {
      if (!digitalRead(BUTTON_PIN)) {
        if (valueCounter == 0) {
          sw_1 = data.command;
          EEPROM.write(1, data.command);
          doublePeep();
          valueCounter++;
        }
        if (valueCounter == 1) {
          sw_2 = data.command;
          EEPROM.write(2, data.command);
          doublePeep();
          valueCounter++;
        }
        if (valueCounter == 2) {
          sw_p = data.command;
          EEPROM.write(3, data.command);
          doublePeep();
          valueCounter++;
        }
        if (valueCounter == 3) {
          sw_n = data.command;
          EEPROM.write(4, data.command);
          doublePeep();
          valueCounter = 0;
        }
      }
    }
  }
}
