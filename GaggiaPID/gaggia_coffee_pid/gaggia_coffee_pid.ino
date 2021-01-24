#include <SSD1306Wire.h>
#include <Adafruit_MAX31865.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

// Use software SPI: CS, DI, DO, CLK
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(10, 11, 12, 13);
// use hardware SPI, just pass in the CS pin
Adafruit_MAX31865 thermo = Adafruit_MAX31865(2);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

SSD1306Wire display(0x3c, D2, D3);

const char* ssid = "<SSID>";
const char* password = "<PASS>";

ESP8266WebServer server(80);

float temperature = 35;
float actualTemperature = 20;
bool isBoiling = false;

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleRoot() {
  String boilingStatus = isBoiling ? "True" : "False";
  server.send(200, "text/html;charset=UTF-8", "<HTML>"
  "<HEAD>"
  "<TITLE>Gaggia Coffee PID</TITLE>"
  "</HEAD>"
  "<BODY><LEFT><B><P>Temperature: " + String(temperature, 2) + " C°</P><P>Current temperature:" + String(actualTemperature, 2) + " C°</P></B></LEFT>"
  "<p><a href=\"incTemp\">+ C°</a></p>"
  "<p><a href=\"decTemp\">- C°</a></p>"
  "<LEFT><B><P>Boiling: " + boilingStatus  + "</P></B></LEFT>"
  "</BODY>"
  "</HTML>");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Gaggia PID");
  pinMode(D1, OUTPUT);
  digitalWrite(D1, LOW);
  isBoiling = false;

  EEPROM.begin(4);
  EEPROM.get(0, temperature);
  Serial.println("Temp read from EEPROM: " + String(temperature,2));
  if (temperature < 20 || temperature > 150) {
    temperature = 35;
    EEPROM.put(0, temperature);
    Serial.println("Temperature written to EEPROM");
    EEPROM.get(0, temperature);
    Serial.println("Temp read from EEPROM: " + String(temperature,2));
    EEPROM.end();
  }

  thermo.begin(MAX31865_2WIRE);  // set to 2WIRE or 4WIRE as necessary

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/incTemp", []() {
    temperature += 1;
    SaveTemperatureToEEPROM();
    handleRoot();
  });

  server.on("/decTemp", []() {
    temperature -= 1;
    SaveTemperatureToEEPROM();
    handleRoot();
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
}


void loop() {
  uint16_t rtd = thermo.readRTD();

  Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;
  Serial.print("Ratio = "); Serial.println(ratio,8);
  Serial.print("Resistance = "); Serial.println(RREF*ratio,8);
  actualTemperature = thermo.temperature(RNOMINAL, RREF);
  Serial.print("Temperature = "); Serial.println(actualTemperature);
  if (actualTemperature < temperature) { 
    digitalWrite(D1, HIGH);
    isBoiling = true;
  }
  else {
    digitalWrite(D1, LOW);
    isBoiling = false;
  }

  // Check and print any faults
  uint8_t fault = thermo.readFault();
  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    thermo.clearFault();
  }
  Serial.println();
  delay(1000);

  server.handleClient();
  MDNS.update();

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 0, "T: " + String(temperature,2) + "C°");
  display.drawString(0, 24, "A: " + String(actualTemperature,2) + "C°");
  display.display();  
}

void SaveTemperatureToEEPROM() {
  EEPROM.begin(4);
  EEPROM.put(0, temperature);
  EEPROM.end();    
  Serial.print("Temperature increased to: "); Serial.println(temperature);
}
