#include <Wire.h>

#include <Sodaq_BMP085.h>

#include <SimpleDHT.h>

#include <ThingSpeak.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

  char ssid[] = "******";    //  your network SSID (name) 
  char pass[] = "**********";   // your network password
  int status = WL_IDLE_STATUS;
  WiFiClient  client;
  
  unsigned long myChannelNumber = 23; // channel num from thingspeak;
  const char * myWriteAPIKey = "channel write api key";

  SimpleDHT22 dht22;
  Sodaq_BMP085 bmp;

void getVoltageAndUpload(float* voltage) {
  float voltageAggr = 0;
  for (int i = 0; i< 3; i++){
    int val = analogRead(A0);
    Serial.print("Analog read: ");Serial.println(val);
    voltageAggr += val;
    delay(10);
  }
  *voltage = voltageAggr / 3;
  Serial.println((float)*voltage);
}

int readTempAndHum(float* temperature, float* humidity ) {
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(D2, temperature, humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.println("Read DHT22 failed, err="); Serial.println(err);delay(2000);
    return -1;
  }
  Serial.print((float)*temperature); Serial.print(" *C, ");
  Serial.print((float)*humidity); Serial.println(" RH%");
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
  Wire.pins(D1, D4);
  Wire.begin(D1, D4);
  bmp.begin();
  
  ThingSpeak.begin(client);

  loop();

  ESP.deepSleep(600e6);
}

void loop() {
  float voltage = 0;
  float temperature = 0;
  float humidity = 0;
  getVoltageAndUpload(&voltage);
  ThingSpeak.setField(1, (float)voltage);

  if (readTempAndHum(&temperature, &humidity) != -1) { 
    ThingSpeak.setField(2, (float)temperature);
    ThingSpeak.setField(3, (float)humidity);
  }
  float temperature2 = bmp.readTemperature();
  int32_t pressureAggr = bmp.readPressure();
  delay(10);
  pressureAggr += bmp.readPressure();
  delay(10);
  pressureAggr += bmp.readPressure();
  int32_t pressure = pressureAggr/3;
  Serial.print(temperature2); Serial.print(" *C ");
  Serial.print(pressure); Serial.println(" Pa ");
  ThingSpeak.setField(4, (float)temperature2);
  ThingSpeak.setField(5, pressure);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  //Serial.print(bmp.readAltitude(102000)); Serial.println(" m");
}
