#include <ESP8266WiFi.h>

const char* ssid = "Smartha 6.1";
const char* password = "smartha_kokowawa";

WiFiServer server(80);

void setup() {
 Serial.begin(9600);
 //Serial.swap();
 //Serial1.setDebugOutput(true);

  //WiFi.mode(WIFI_STA);
  WiFi.softAP(ssid, password);

  // Start the server
  delay(1000);
  server.begin();
  Serial.println(0xB);
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  
  if (req.indexOf(F("1/")) != -1) {
    Serial.println(1);
  }
  else if (req.indexOf(F("2/")) != -1) {
    Serial.println(2);
  }
  else if (req.indexOf(F("3/")) != -1) {
    Serial.println(3);
  }
  else if (req.indexOf(F("4/")) != -1) {
    Serial.println(4);
  }

  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: application/json"));
}
