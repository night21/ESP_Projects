#include <NTPClient_Generic.h>
#include <NTPClient_Generic.hpp>
#include <WiFiUdp.h>

#include <CustomJWT.h>

#include <MQ7.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <DHT.h>
#include <DHT_U.h>

// DHT configuration
#define DHTPIN 4     // D2 - 4 Digital pin connected to the DHT sensor
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// Wifi configuration
#define SERVER_IP "<IP>"
#define SERVER_PORT "<PORT>"
#define STASSID "<SSID>"
#define STAPSK "<PASS>"
// Message Encrytion - HMAC 256 with JWT
char key[] = "<SECRET>";
CustomJWT jwt(key, 256);

// MQ-7 Configuration
#define A_PIN A0
#define VOLTAGE 5

// init MQ7 device
MQ7 mq7(A_PIN, VOLTAGE);

// configure NTP
WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP);
int counter = 0;

void setup() {
  
  Serial.begin(9600);
  Serial.println(F("DHT22 Init!"));

  dht.begin();

  Serial.println("Calibrating MQ7");
  mq7.calibrate();    // calculates R0
  Serial.println("Calibration done!");

  Serial.println(F("Wifi init!"));
  WiFi.begin(STASSID, STAPSK);
  counter = 0;
  while (WiFi.status() != WL_CONNECTED && counter < 30) {
    delay(500);
    Serial.print(".");
    counter++;
  }
  Serial.println("Check Wifi again");
  counter = 0;
  while (WiFi.status() != WL_CONNECTED && counter < 30) {
    delay(500);
    Serial.print(".");
    counter++;
  }
  delay(1000);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  counter = 0;
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());  


  timeClient.begin();
  Serial.println("Using NTP Server " + timeClient.getPoolServerName());

  timeClient.update();
  counter = 0;
  while(timeClient.getYear() == 1970 && counter < 30) 
  {
    delay(1000);
    timeClient.update();
    Serial.print(".");
    counter++;
  }
  counter = 0;
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));

  float ppm = mq7.readPpm();
  Serial.print("PPM = "); Serial.println(ppm);

  char tt[7];
  char ht[7];
  char pt[7];

  dtostrf(t, 5, 2, tt);
  dtostrf(h, 5, 2, ht);
  dtostrf(ppm, 5, 2, pt);

  char rawMessage[93];
  sprintf(rawMessage, "{\"time\":\"%04d-%02d-%02d %02d:%02d:%02d\", \"temperature\":%s, \"humidity\":%s, \"co\":%s}", 
  timeClient.getYear(),
  timeClient.getMonth(),
  timeClient.getDay(), 
  timeClient.getHours(),
  timeClient.getMinutes(),
  timeClient.getSeconds(),
  tt, ht, pt);

  Serial.print("Message: ");
  Serial.println(rawMessage);
  
  jwt.allocateJWTMemory();
  jwt.encodeJWT(rawMessage);

  //Serial.println("Raw Message: ");
  //Serial.println(rawMessage);
  //Serial.println(jwt.out);
  if ((WiFi.status() != WL_CONNECTED)) {
    Serial.println("Reconnecting to WIFI network");
    WiFi.disconnect();
    WiFi.begin(STASSID, STAPSK);  
    while (WiFi.status() != WL_CONNECTED && counter < 30) {
      delay(500);
      Serial.print(".");
      counter++;
    }
    counter = 0;
  }
  
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP ":" SERVER_PORT);  // HTTP
    http.addHeader("Content-Type", "text/html");


    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = http.POST(jwt.out);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        //Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println(WiFi.status());
  }
  jwt.clear();
  Serial.println("Going Deep");
  ESP.deepSleep(300e6);
  // Wait a few seconds between measurements.
  //delay(300000);
  
}

void loop() { }
