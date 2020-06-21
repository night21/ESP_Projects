#include <HCSR04.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define S_ON 1
#define S_OFF 0
const char* ssid = "<SSID>"; // SSID
const char* password = "<Password>";

ESP8266WebServer server(80);
HCSR04 hc(5,4);//initialisation class HCSR04 (trig pin , echo pin)

IPAddress ip(192, 168, 1, 125);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

int a1a = 12;
int a1b = 14;
int b1a = 15;
int b1b = 13;
int globalStop = 0;

int state = S_OFF;

String motorStatus = "Started";
void motorStop();
void motorForward();
void motorReverse();

void handleRoot() {
  String stateString = state ? "On" : "Off";
  server.send(200, "text/html", "<HTML>"
  "<HEAD>"
  "<TITLE>Car status</TITLE>"
  "</HEAD>"
  "<BODY><LEFT><B><P> Car direction: " + motorStatus + "</P><P> Car state:" + stateString + "</P></B></LEFT>"
  "<p><a href=\"start\">Start car</a></p>"
  "<p><a href=\"stop\">Stop car</a></p>"
  "</BODY>"
  "</HTML>");
}

void handleStart() {
  state = S_ON;
  Serial.println("State turned on");
  handleRoot();
}

void handleStop() {
  state = S_OFF;
  Serial.println("State turned off");
  handleRoot();
}

void setup()
{ 
  Serial.begin(9600); 
  delay(100);
  pinMode(a1a, OUTPUT);
  pinMode(a1b, OUTPUT);
  pinMode(b1a, OUTPUT);
  pinMode(b1b, OUTPUT);
  motorStop();

  WiFi.config(ip, gateway, subnet);
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
  server.on("/start", handleStart);
  server.on("/stop", handleStop);
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{ 
  server.handleClient();
  if (state == S_ON) {
    float distance = hc.dist();
    if (distance < 40 && distance > 10) {
      Serial.println(distance);
      int count = 0;
      while (hc.dist() < 40 && count < 10) {
      motorTurnLeft();
      delay(100);
      }
      motorStop();
    } else if (distance < 10)  {
      motorReverse();
    }  else  {
      motorForward();
    }
  }
  else {
    motorStop();
  }
  delay(100);
}

void motorStop() {
      analogWrite(a1a,0);
      analogWrite(b1a,0);
      digitalWrite(a1b,LOW);
      digitalWrite(b1b,LOW);
      motorStatus = "Stopped";
}

void motorReverse() {
      analogWrite(a1a,600);
      analogWrite(b1a,600);
      digitalWrite(a1b,LOW);
      digitalWrite(b1b,LOW);
      motorStatus = "Reverse";
}

void motorForward() {
      digitalWrite(a1b,HIGH);
      digitalWrite(b1b,HIGH);
      analogWrite(a1a,2000);
      analogWrite(b1a,2000);
      motorStatus = "Forward";
}

void motorTurnLeft() {
      digitalWrite(a1b,HIGH);
      digitalWrite(b1b,LOW);
      analogWrite(a1a,2000);
      analogWrite(b1a,0);
      motorStatus = "Forward";
}
