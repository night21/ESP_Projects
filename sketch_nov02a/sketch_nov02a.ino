#include <ThingSpeak.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

  #define VOLTAGE_MAX 1.0
  #define VOLTAGE_MAXCOUNTS 1023.0
  
  char ssid[] = "<SSID>";    //  your network SSID (name) 
  char pass[] = "<WIFI PASS>";   // your network password
  int status = WL_IDLE_STATUS;
  WiFiClient  client;
  
  unsigned long myChannelNumber = <ThingSpeak Channel num>;
  const char * myWriteAPIKey = "<Channel API Key>";

void getVoltageAndUpload() {
  Serial.println("Loop started");
  int val = analogRead(A0);
  float voltage = val * (VOLTAGE_MAX / VOLTAGE_MAXCOUNTS);
  Serial.println(voltage);
  ThingSpeak.writeField(myChannelNumber, 1, voltage, myWriteAPIKey);  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while (!Serial) {};
  Serial.println("Setup ready");
  WiFi.begin(ssid, pass);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  ThingSpeak.begin(client);
  getVoltageAndUpload();
  ESP.deepSleep(3600e6);
}

void loop() {
}
